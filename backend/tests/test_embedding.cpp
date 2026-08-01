#include "../core/Embedding/Embedding.h"
#include "../common/EnvUtil.h"
#include <iostream>
#include <cstdlib>
#include <chrono>

using namespace std;

int main() {
    string apiKey = loadEnvVar("GEMINI_API_KEY");
    if (apiKey.empty()) {
        cerr << "Error: GEMINI_API_KEY not set (env var or .env file)." << endl;
        return 1;
    }

    Embedding api(apiKey);

    cout << "Requesting embedding for: 'happy'..." << endl;
    Vector<float> embedding = api.getEmbedding("happy");

    if (embedding.empty()) {
        cerr << "Error: Received empty embedding. Check API key and internet connection." << endl;
        return 1;
    }

    cout << "Success!" << endl;
    cout << "Vector Size: " << embedding.length()<< " dimensions" << endl;


    for (size_t i = 0; i < embedding.length(); ++i) {
        cout << i+1<<": "<< embedding[i] << (i == 4 ? "" : ", ");
    }
    cout << "..." << endl;

    cout << "\n=== In-memory cache test ===" << endl;
    cout << "Cache size after 1 unique request: " << api.getCacheSize() << " (expected: 1)" << endl;

    auto start = chrono::steady_clock::now();
    Vector<float> cachedEmbedding = api.getEmbedding("happy"); // should hit the in-memory cache, no network call
    auto elapsedMs = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count();

    bool sameValues = cachedEmbedding.length() == embedding.length();
    for (int i = 0; sameValues && i < embedding.length(); i++) {
        if (cachedEmbedding[i] != embedding[i]) sameValues = false;
    }

    cout << "Repeated request for 'happy' returned in " << elapsedMs << " ms" << endl;
    cout << "Cache size after repeat request: " << api.getCacheSize() << " (expected: still 1, not 2)" << endl;
    cout << ((sameValues && api.getCacheSize() == 1 && elapsedMs < 50) ? "PASS" : "FAIL")
         << ": repeated request served from cache (identical values, no growth, no network latency)" << endl;

    return 0;
}
