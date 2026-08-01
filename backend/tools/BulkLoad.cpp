// Embeds every word in a vocabulary file through the live Gemini API,
// inserts each into a VectorDB, and persists it to disk. Also writes a
// sidecar word list — VectorDB::save()/load() doesn't persist per-item
// metadata (see build-plan.md), so a plain text file with one word per
// line, in exact insertion order (line 1 == id 0, line 2 == id 1, ...),
// is how the Crow server maps result ids back to words. That mapping is
// only valid because this tool assigns ids by insertion order and never
// skips an id once assigned — see the loop below.
//
// Usage: ./bulkload [wordlist] [output.vdb] [output_words.txt]
// Defaults: backend/data/vocabulary.txt, backend/data/vocabulary.vdb,
//           backend/data/vocabulary_words.txt
// Run from the repo root (same cwd rules as backend/main.cpp).

#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include "../core/vector/VectorDB.h"
#include "../core/Embedding/Embedding.h"
#include "../common/EnvUtil.h"
#include "../EmbeddingCache.h"

using namespace std;

int main(int argc, char** argv) {
    string wordlistPath = argc > 1 ? argv[1] : "backend/data/vocabulary.txt";
    string outputVdb = argc > 2 ? argv[2] : "backend/data/vocabulary.vdb";
    string outputWords = argc > 3 ? argv[3] : "backend/data/vocabulary_words.txt";

    string apiKey = loadEnvVar("GEMINI_API_KEY", ".env");
    if (apiKey.empty()) {
        cerr << "GEMINI_API_KEY not set. Run from the repo root with a .env file present.\n";
        return 1;
    }

    ifstream wordFile(wordlistPath);
    if (!wordFile.is_open()) {
        cerr << "Could not open word list: " << wordlistPath << "\n";
        return 1;
    }

    Embedding api(apiKey);
    EmbeddingCache cache(".embedding_cache");
    VectorDB db;
    ofstream wordsOut(outputWords);

    string word;
    int lineNum = 0, inserted = 0, failed = 0;
    auto start = chrono::steady_clock::now();

    while (getline(wordFile, word)) {
        lineNum++;
        if (word.empty()) continue;

        Vector<float> emb;
        if (!cache.find(word, emb)) {
            emb = api.getEmbedding(word);
            if (emb.empty()) {
                // One retry after a short backoff — handles transient
                // network/API blips without slowing the whole run down.
                this_thread::sleep_for(chrono::milliseconds(1000));
                emb = api.getEmbedding(word);
            }
            if (!emb.empty()) cache.put(word, emb);
        }

        if (emb.empty()) {
            cerr << "  [SKIP] failed to embed '" << word << "' after retry\n";
            failed++;
            continue;
        }

        db.insert(emb);
        wordsOut << word << "\n";
        inserted++;

        if (inserted % 50 == 0) {
            double elapsedSec = chrono::duration_cast<chrono::duration<double>>(
                chrono::steady_clock::now() - start).count();
            cout << "  " << inserted << " embedded (" << failed << " failed), "
                 << (int)elapsedSec << "s elapsed, line " << lineNum << "\n";
        }
    }

    wordsOut.close();

    cout << "\nDone: " << inserted << " embedded, " << failed << " failed, "
         << db.size() << " in DB.\n";

    if (!db.save(outputVdb)) {
        cerr << "Failed to save to " << outputVdb << "\n";
        return 1;
    }
    cout << "Saved to " << outputVdb << "\n";
    cout << "Word list saved to " << outputWords << "\n";

    return 0;
}
