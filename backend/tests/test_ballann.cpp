#include <iostream>
#include <cstdlib>
#include <ctime>
#include "../core/vector/VectorDB.h"
#include "../core/vector/VectorItem.h"
#include "../core/vector/BallANN.h"
#include "../core/vector/Similarity.h"

using namespace std;

Vector<float> randomVector(int dim) {
    Vector<float> v;
    for (int i = 0; i < dim; i++) {
        v.push_back(((float)(rand() % 2000) / 1000.0f) - 1.0f); // [-1, 1)
    }
    return v;
}

Vector<Pair<int, float>> bruteForceEuclidean(VectorDB& db, const Vector<float>& query, int k) {
    Vector<Pair<float, int>> all;
    for (int i = 0; i < db.size(); i++) {
        VectorItem* item = db.at(i);
        float d = Similarity::euclideanDistance(query, item->embedding);
        all.push_back(Pair<float, int>(d, item->id));
    }
    Vector<Pair<int, float>> results;
    for (int pick = 0; pick < k && pick < all.length(); pick++) {
        int best = pick;
        for (int i = pick + 1; i < all.length(); i++) {
            if (all[i].key < all[best].key) best = i;
        }
        all.swap(pick, best);
        results.push_back(Pair<int, float>(all[pick].value, all[pick].key));
    }
    return results;
}

void testBasicBuildAndSearch() {
    cout << "=== Test 1: Basic Build and Search ===" << endl;
    VectorDB db;
    Vector<float> a; a.push_back(1); a.push_back(0); a.push_back(0);
    Vector<float> b; b.push_back(0); b.push_back(1); b.push_back(0);
    Vector<float> c; c.push_back(0.9f); c.push_back(0.1f); c.push_back(0);

    db.insert(a);
    db.insert(b);
    db.insert(c);

    BallANN ball(&db);
    ball.buildTree();

    Vector<Pair<int, float>> results = ball.search(a, 2);
    cout << "Query = a (1,0,0), top 2:" << endl;
    for (int i = 0; i < results.length(); i++) {
        cout << "  id=" << results[i].key << " dist=" << results[i].value << endl;
    }
    cout << (results.length() == 2 && results[0].key == 0 ? "PASS" : "FAIL")
         << ": nearest is the exact match (id 0, dist 0)" << endl;

    cout << endl;
}

void testMatchesExhaustiveEuclideanSearch() {
    cout << "=== Test 2: Matches Exhaustive Euclidean Search (n=300, d=16) ===" << endl;
    srand(7);
    VectorDB db;
    int n = 300, dim = 16;
    for (int i = 0; i < n; i++) db.insert(randomVector(dim));

    BallANN ball(&db, 6);
    ball.buildTree();

    int trials = 15, k = 5;
    int mismatches = 0;

    for (int t = 0; t < trials; t++) {
        Vector<float> query = randomVector(dim);

        Vector<Pair<int, float>> ballResults = ball.search(query, k);
        Vector<Pair<int, float>> bfResults = bruteForceEuclidean(db, query, k);

        if (ballResults.length() != bfResults.length()) {
            mismatches++;
            continue;
        }
        for (int i = 0; i < ballResults.length(); i++) {
            if (ballResults[i].key != bfResults[i].key) {
                mismatches++;
                break;
            }
        }
    }

    cout << "Ran " << trials << " random queries, k=" << k << endl;
    cout << (mismatches == 0 ? "PASS" : "FAIL")
         << ": BallANN top-k ids exactly match brute-force Euclidean top-k ("
         << (trials - mismatches) << "/" << trials << " trials matched)" << endl;

    cout << endl;
}

void testEmptyDatabase() {
    cout << "=== Test 3: Empty Database ===" << endl;
    VectorDB db;
    BallANN ball(&db);
    ball.buildTree();

    Vector<float> query; query.push_back(1); query.push_back(2);
    Vector<Pair<int, float>> results = ball.search(query, 5);

    cout << "Results on empty DB: " << results.length() << " (expected: 0)" << endl;
    cout << (results.length() == 0 ? "PASS" : "FAIL") << ": empty database handled gracefully" << endl;

    cout << endl;
}

void testKLargerThanDatabase() {
    cout << "=== Test 4: k Larger Than Database Size ===" << endl;
    VectorDB db;
    Vector<float> a; a.push_back(1); a.push_back(1);
    Vector<float> b; b.push_back(2); b.push_back(2);
    db.insert(a);
    db.insert(b);

    BallANN ball(&db);
    ball.buildTree();

    Vector<float> query; query.push_back(1); query.push_back(1);
    Vector<Pair<int, float>> results = ball.search(query, 10);

    cout << "Requested k=10 on a 2-vector DB, got " << results.length() << " results (expected: 2)" << endl;
    cout << (results.length() == 2 ? "PASS" : "FAIL") << ": returns all available instead of crashing" << endl;

    cout << endl;
}

void testSmallerThanLeafSize() {
    cout << "=== Test 5: Database Smaller Than Leaf Size (single leaf, no split) ===" << endl;
    srand(99);
    VectorDB db;
    for (int i = 0; i < 5; i++) db.insert(randomVector(8));

    BallANN ball(&db, 8); // leafSize=8 >= n=5, tree is a single leaf node
    ball.buildTree();

    Vector<float> query = randomVector(8);
    Vector<Pair<int, float>> ballResults = ball.search(query, 3);
    Vector<Pair<int, float>> bfResults = bruteForceEuclidean(db, query, 3);

    bool match = ballResults.length() == bfResults.length();
    for (int i = 0; match && i < ballResults.length(); i++) {
        if (ballResults[i].key != bfResults[i].key) match = false;
    }
    cout << (match ? "PASS" : "FAIL") << ": single-leaf tree still matches brute force" << endl;

    cout << endl;
}

int main() {
    cout << "======================================" << endl;
    cout << "    BallANN (Ball Tree) Test Suite" << endl;
    cout << "======================================" << endl;
    cout << endl;

    testBasicBuildAndSearch();
    testMatchesExhaustiveEuclideanSearch();
    testEmptyDatabase();
    testKLargerThanDatabase();
    testSmallerThanLeafSize();

    cout << "======================================" << endl;
    cout << "    All Tests Complete" << endl;
    cout << "======================================" << endl;

    return 0;
}
