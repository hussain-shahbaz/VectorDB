// HTTP API for the synonym demo: loads a persisted VectorDB + its word
// sidecar file (see backend/tools/BulkLoad.cpp for why a sidecar file
// rather than persisted metadata), builds a GraphANN index, and serves
// nearest-neighbor "synonym" lookups over HTTP for the React frontend.
//
// Usage: ./synonym_server [vocabulary.vdb] [vocabulary_words.txt] [port]
// Run from the repo root (same cwd rules as backend/main.cpp — .env and
// the given paths are resolved relative to the process's cwd).

#include "crow_all.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>

#include "../core/vector/VectorDB.h"
#include "../core/vector/SearchEngine.h"
#include "../core/vector/GraphANN.h"
#include "../core/Embedding/Embedding.h"
#include "../common/EnvUtil.h"
#include "../EmbeddingCache.h"

using namespace std;

int main(int argc, char** argv) {
    string vdbPath = argc > 1 ? argv[1] : "backend/data/vocabulary.vdb";
    string wordsPath = argc > 2 ? argv[2] : "backend/data/vocabulary_words.txt";
    int port = argc > 3 ? atoi(argv[3]) : 8080;

    string apiKey = loadEnvVar("GEMINI_API_KEY", ".env");
    if (apiKey.empty()) {
        cerr << "GEMINI_API_KEY not set. Run from the repo root with a .env file present.\n";
        return 1;
    }

    VectorDB db;
    if (!db.load(vdbPath)) {
        cerr << "Failed to load '" << vdbPath << "'. Run the bulk loader first:\n"
             << "  ./bulkload\n";
        return 1;
    }

    Vector<string> wordsById;
    {
        ifstream wf(wordsPath);
        string line;
        while (getline(wf, line)) {
            if (!line.empty()) wordsById.push_back(line);
        }
    }

    cout << "Loaded " << db.size() << " vectors, " << wordsById.length() << " words from disk.\n";
    if (db.size() != wordsById.length()) {
        cerr << "WARNING: vector count (" << db.size() << ") and word count (" << wordsById.length()
             << ") don't match. Id-to-word mapping will be off — was the word list edited "
             << "after the .vdb was built, or is this the wrong pair of files?\n";
    }

    Embedding api(apiKey, 1000); // larger in-memory cache — a long-lived server sees more distinct queries than a one-shot CLI run
    // Embedding's cache (HashMap + List) isn't internally synchronized, and
    // Crow's multithreaded() mode dispatches concurrent requests to worker
    // threads — without this, two simultaneous queries could race on the
    // cache and corrupt it. SearchEngine::search()/GraphANN::search() are
    // read-only over `db`/`graph` (never mutated after buildGraph() below),
    // so they don't need the same protection.
    mutex embeddingMutex;
    // Disk cache shared with main.cpp/bulkload/benchmark_cpp — a fresh
    // server process has an empty in-memory Embedding cache, so without
    // this, querying a word that's already part of the bulk-loaded
    // vocabulary (or embedded by any other tool in this session) would
    // still cost a live API call. Checking this first means the whole
    // pre-loaded vocabulary is instantly queryable even if the live API
    // quota is exhausted — only genuinely new words need a live call.
    EmbeddingCache diskCache(".embedding_cache");

    SearchEngine engine(&db);
    GraphANN ann(&db, 8);

    cout << "Building GraphANN index...\n";
    ann.buildGraph();
    cout << "Ready — listening on port " << port << "\n";

    crow::App<crow::CORSHandler> app;
    app.get_middleware<crow::CORSHandler>()
        .global()
        .origin("*")
        .methods("GET"_method, "OPTIONS"_method);

    CROW_ROUTE(app, "/api/stats")
    ([&]() {
        crow::json::wvalue res;
        res["wordCount"] = db.size();
        return res;
    });

    CROW_ROUTE(app, "/api/synonyms")
    ([&](const crow::request& req) {
        auto wordParam = req.url_params.get("word");
        if (!wordParam || string(wordParam).empty()) {
            return crow::response(400, R"({"error":"missing or empty 'word' query parameter"})");
        }
        string wordStr(wordParam);

        int k = 10;
        if (auto kParam = req.url_params.get("k")) k = atoi(kParam);
        if (k < 1) k = 1;
        if (k > 50) k = 50;

        string method = "graph";
        if (auto methodParam = req.url_params.get("method")) method = methodParam;

        Vector<float> emb;
        {
            // Also guards diskCache: like Embedding's own cache, it's a
            // plain (non-atomic) Vector-backed cache, not safe for
            // concurrent access from Crow's worker threads.
            lock_guard<mutex> lock(embeddingMutex);
            if (!diskCache.find(wordStr, emb)) {
                emb = api.getEmbedding(wordStr);
                if (!emb.empty()) diskCache.put(wordStr, emb);
            }
        }
        if (emb.empty()) {
            return crow::response(502, R"({"error":"failed to embed the query word"})");
        }

        Vector<Pair<int, float>> results = (method == "brute") ? engine.search(emb, k) : ann.search(emb, k);

        crow::json::wvalue res;
        res["query"] = wordStr;
        res["method"] = method;

        vector<crow::json::wvalue> items;
        for (int i = 0; i < results.length(); i++) {
            int id = results[i].key;
            crow::json::wvalue item;
            item["word"] = (id >= 0 && id < wordsById.length()) ? wordsById[id] : "?";
            item["similarity"] = results[i].value;
            items.push_back(std::move(item));
        }
        res["results"] = std::move(items);

        return crow::response(res);
    });

    app.port(port).multithreaded().run();
    return 0;
}
