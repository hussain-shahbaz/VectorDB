// Recall@k benchmark: GraphANN vs. SearchEngine (brute force = ground
// truth) on synthetic random vectors at increasing scale. Uses synthetic
// data rather than live embeddings (benchmark_cpp.cpp already covers the
// live-API path at n=95) because measuring recall properly needs thousands
// of vectors, and that many live API calls would be slow and wasteful of
// quota for what's fundamentally a question about the graph algorithm, not
// the embedding quality.
//
// Recall@k = |ANN_results ∩ exact_results| / k, per project_documentation.tex.

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include "../core/vector/VectorDB.h"
#include "../core/vector/SearchEngine.h"
#include "../core/vector/GraphANN.h"
#include "../core/vector/Similarity.h"

using namespace std;

Vector<float> randomVector(int dim) {
    Vector<float> v;
    for (int i = 0; i < dim; i++) {
        v.push_back(((float)(rand() % 20000) / 10000.0f) - 1.0f);
    }
    return v;
}

struct BenchResult {
    int n;
    double buildTimeMs;
    double avgBruteMs;
    double avgAnnMs;
    double avgRecall;
};

BenchResult runBenchmark(int n, int dim, int maxConn, int k, int numQueries) {
    srand(42); // fixed seed: comparable numbers across sizes/runs
    VectorDB db;
    for (int i = 0; i < n; i++) db.insert(randomVector(dim));

    SearchEngine brute(&db);
    GraphANN ann(&db, maxConn);

    clock_t buildStart = clock();
    ann.buildGraph();
    clock_t buildEnd = clock();

    double totalBruteMs = 0, totalAnnMs = 0, totalRecall = 0;

    for (int q = 0; q < numQueries; q++) {
        Vector<float> query = randomVector(dim);

        clock_t s1 = clock();
        Vector<Pair<int, float>> bruteRes = brute.search(query, k);
        clock_t e1 = clock();

        clock_t s2 = clock();
        Vector<Pair<int, float>> annRes = ann.search(query, k);
        clock_t e2 = clock();

        totalBruteMs += (double)(e1 - s1) / CLOCKS_PER_SEC * 1000.0;
        totalAnnMs += (double)(e2 - s2) / CLOCKS_PER_SEC * 1000.0;

        int overlap = 0;
        for (int i = 0; i < annRes.length(); i++) {
            for (int j = 0; j < bruteRes.length(); j++) {
                if (annRes[i].key == bruteRes[j].key) overlap++;
            }
        }
        totalRecall += (double)overlap / k;
    }

    BenchResult r;
    r.n = n;
    r.buildTimeMs = (double)(buildEnd - buildStart) / CLOCKS_PER_SEC * 1000.0;
    r.avgBruteMs = totalBruteMs / numQueries;
    r.avgAnnMs = totalAnnMs / numQueries;
    r.avgRecall = totalRecall / numQueries;
    return r;
}

int main() {
    int dim = 32, maxConn = 10, k = 10, numQueries = 30;
    int sizes[] = {1000, 3000, 5000, 8000};
    int numSizes = 4;

    cout << "====================================\n";
    cout << " GraphANN Recall@" << k << " Benchmark\n";
    cout << " d=" << dim << ", maxConnections=" << maxConn << ", " << numQueries << " queries/size\n";
    cout << "====================================\n\n";

    cout << left
         << setw(8) << "n"
         << setw(16) << "BuildTime(ms)"
         << setw(14) << "Brute(ms)"
         << setw(12) << "ANN(ms)"
         << setw(10) << "Speedup"
         << "Recall@" << k << "\n";
    cout << string(70, '-') << "\n";

    for (int i = 0; i < numSizes; i++) {
        BenchResult r = runBenchmark(sizes[i], dim, maxConn, k, numQueries);
        double speedup = r.avgAnnMs > 0 ? r.avgBruteMs / r.avgAnnMs : 0;

        cout << left
             << setw(8) << r.n
             << setw(16) << r.buildTimeMs
             << setw(14) << r.avgBruteMs
             << setw(12) << r.avgAnnMs
             << setw(10) << speedup
             << r.avgRecall << "\n";
    }

    cout << "\nTarget from project report: recall@10 > 0.90 with >10x speedup at scale.\n";
    cout << "Note: at n in the low thousands with maxConnections=10, the graph is\n";
    cout << "dense relative to n, so ANN speedup is modest until n grows further —\n";
    cout << "brute force is O(nd) per query and doesn't get *slow* until n is large.\n";

    return 0;
}
