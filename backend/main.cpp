// CLI for VectorDB. Two families of usage:
//
//   ./vectordb                     seed the demo corpus, interactive query REPL
//   ./vectordb <word> [k]          seed the demo corpus, one-shot query
//   ./vectordb save <path>         seed the demo corpus, persist it to <path>
//   ./vectordb load <path> [word] [k]   load a persisted DB, query it (by id —
//                                        see note below), REPL if no word given
//   ./vectordb insert <path> <text>     embed <text>, insert into the DB at
//                                        <path> (creating it if missing), save
//
// Run from the repo root — .env, .embedding_cache, and any <path> you pass
// are resolved relative to the current working directory.
//
// Note on `load`: VectorDB::save() doesn't persist per-item metadata (see
// build-plan.md), so a loaded DB has no word text to show — query results
// from `load` print numeric ids, not words. Only the `save` path (which
// seeds the in-memory demo corpus first) can show words, since that mapping
// only ever lived in this process's `wordsById` array.
//
// Requires GEMINI_API_KEY in a .env file at the repo root (see README.md).

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>

#include "core/vector/VectorDB.h"
#include "core/vector/VectorItem.h"
#include "core/vector/SearchEngine.h"
#include "core/vector/GraphANN.h"
#include "core/vector/Similarity.h"
#include "core/Embedding/Embedding.h"
#include "common/EnvUtil.h"
#include "EmbeddingCache.h"

using namespace std;

static const string CORPUS[] = {
    "apple", "banana", "mango", "orange", "grape",
    "computer", "laptop", "software", "algorithm", "database",
    "dog", "cat", "lion", "elephant", "tiger",
    "car", "truck", "bicycle", "airplane", "train",
    "happy", "sad", "angry", "excited", "calm"
};
static const int CORPUS_SIZE = sizeof(CORPUS) / sizeof(CORPUS[0]);

static Vector<float> embedOrCache(Embedding& api, EmbeddingCache& cache, const string& text) {
    Vector<float> cached;
    if (cache.find(text, cached)) return cached;

    Vector<float> fresh = api.getEmbedding(text);
    if (!fresh.empty()) cache.put(text, fresh);
    return fresh;
}

static void printResultsByWord(const string& label, const Vector<Pair<int, float>>& results, const Vector<string>& wordsById) {
    cout << "\n" << label << ":\n";
    if (results.length() == 0) {
        cout << "  (no results)\n";
        return;
    }
    for (int i = 0; i < results.length(); i++) {
        int id = results[i].key;
        float sim = results[i].value;
        string word = (id >= 0 && id < wordsById.length()) ? wordsById[id] : "?";
        cout << "  " << (i + 1) << ". " << word
             << "  (similarity: " << fixed << setprecision(4) << sim << ")\n";
    }
}

static void printResultsById(const string& label, const Vector<Pair<int, float>>& results) {
    cout << "\n" << label << ":\n";
    if (results.length() == 0) {
        cout << "  (no results)\n";
        return;
    }
    for (int i = 0; i < results.length(); i++) {
        cout << "  " << (i + 1) << ". id=" << results[i].key
             << "  (similarity: " << fixed << setprecision(4) << results[i].value << ")\n";
    }
}

static int seedCorpus(VectorDB& db, Embedding& api, EmbeddingCache& cache, Vector<string>& wordsById) {
    cout << "Seeding corpus (" << CORPUS_SIZE << " words)...\n";
    for (int i = 0; i < CORPUS_SIZE; i++) {
        string word = CORPUS[i];
        Vector<float> emb = embedOrCache(api, cache, word);
        if (emb.empty()) {
            cout << "  WARN: failed to embed '" << word << "', skipping.\n";
            continue;
        }
        int id = db.insert(emb);
        wordsById.push_back(word);
        cout << "  [" << id << "] " << word << "\n";
    }
    cout << "Seeded " << db.size() << " / " << CORPUS_SIZE << " vectors.\n";
    return db.size();
}

static int runSave(Embedding& api, EmbeddingCache& cache, const string& path) {
    VectorDB db;
    Vector<string> wordsById;
    if (seedCorpus(db, api, cache, wordsById) == 0) {
        cerr << "Nothing was seeded successfully; aborting.\n";
        return 1;
    }

    if (!db.save(path)) {
        cerr << "Failed to save to '" << path << "'.\n";
        return 1;
    }
    cout << "Saved " << db.size() << " vectors to '" << path << "'.\n";
    return 0;
}

static int runLoad(Embedding& api, EmbeddingCache& cache, const string& path, int argc, char** argv) {
    VectorDB db;
    if (!db.load(path)) {
        cerr << "Failed to load '" << path << "' (missing, or not a valid VectorDB file).\n";
        return 1;
    }
    cout << "Loaded " << db.size() << " vectors from '" << path << "'.\n";
    if (db.size() == 0) {
        cerr << "Loaded database is empty; nothing to search.\n";
        return 1;
    }

    SearchEngine engine(&db);
    GraphANN ann(&db, 5);
    ann.buildGraph();

    int k = 5;
    if (argc > 4) k = atoi(argv[4]);

    auto runQuery = [&](const string& query) {
        Vector<float> qEmb = embedOrCache(api, cache, query);
        if (qEmb.empty()) {
            cout << "Failed to embed query '" << query << "'.\n";
            return;
        }
        cout << "\n=== Query: \"" << query << "\" (top " << k << ") ===\n";
        printResultsById("Brute-force (SearchEngine, exact)", engine.search(qEmb, k));
        printResultsById("GraphANN (approximate)", ann.search(qEmb, k));
    };

    if (argc > 3) {
        runQuery(argv[3]);
        return 0;
    }

    cout << "\nType a word to search (empty line or 'exit' to quit):\n";
    string line;
    while (true) {
        cout << "> ";
        if (!getline(cin, line)) break;
        if (line.empty() || line == "exit" || line == "quit") break;
        runQuery(line);
    }
    return 0;
}

static int runInsert(Embedding& api, EmbeddingCache& cache, const string& path, const string& text) {
    VectorDB db;
    if (!db.load(path)) {
        cout << "'" << path << "' doesn't exist yet (or isn't a valid file) — starting a new database.\n";
    } else {
        cout << "Loaded " << db.size() << " existing vectors from '" << path << "'.\n";
    }

    Vector<float> emb = embedOrCache(api, cache, text);
    if (emb.empty()) {
        cerr << "Failed to embed '" << text << "'.\n";
        return 1;
    }

    int id = db.insert(emb);
    if (!db.save(path)) {
        cerr << "Embedded and inserted (id=" << id << ") but failed to save to '" << path << "'.\n";
        return 1;
    }

    cout << "Inserted \"" << text << "\" as id " << id << ", saved " << db.size() << " vectors to '" << path << "'.\n";
    return 0;
}

int main(int argc, char** argv) {
    string apiKey = loadEnvVar("GEMINI_API_KEY", ".env");
    if (apiKey.empty()) {
        cerr << "GEMINI_API_KEY not set. Put it in a .env file at the repo root "
             << "(GEMINI_API_KEY=...) or export it, then run this from the repo root.\n";
        return 1;
    }

    Embedding api(apiKey);
    EmbeddingCache cache(".embedding_cache");

    string command = argc > 1 ? argv[1] : "";

    if (command == "save") {
        if (argc < 3) { cerr << "Usage: vectordb save <path>\n"; return 1; }
        return runSave(api, cache, argv[2]);
    }
    if (command == "load") {
        if (argc < 3) { cerr << "Usage: vectordb load <path> [word] [k]\n"; return 1; }
        return runLoad(api, cache, argv[2], argc, argv);
    }
    if (command == "insert") {
        if (argc < 4) { cerr << "Usage: vectordb insert <path> <text>\n"; return 1; }
        return runInsert(api, cache, argv[2], argv[3]);
    }

    // Default: seed the demo corpus, then query — `./vectordb [word] [k]`.
    VectorDB db;
    Vector<string> wordsById;
    if (seedCorpus(db, api, cache, wordsById) == 0) {
        cerr << "Nothing was seeded successfully; aborting.\n";
        return 1;
    }

    SearchEngine engine(&db);
    GraphANN ann(&db, 5);
    ann.buildGraph();

    int k = 5;
    if (argc > 2) k = atoi(argv[2]);

    auto runQuery = [&](const string& query) {
        Vector<float> qEmb = embedOrCache(api, cache, query);
        if (qEmb.empty()) {
            cout << "Failed to embed query '" << query << "'.\n";
            return;
        }
        cout << "\n=== Query: \"" << query << "\" (top " << k << ") ===\n";
        printResultsByWord("Brute-force (SearchEngine, exact)", engine.search(qEmb, k), wordsById);
        printResultsByWord("GraphANN (approximate)", ann.search(qEmb, k), wordsById);
    };

    if (argc > 1) {
        runQuery(argv[1]);
        return 0;
    }

    cout << "\nType a word to search (empty line or 'exit' to quit):\n";
    string line;
    while (true) {
        cout << "> ";
        if (!getline(cin, line)) break;
        if (line.empty() || line == "exit" || line == "quit") break;
        runQuery(line);
    }

    return 0;
}
