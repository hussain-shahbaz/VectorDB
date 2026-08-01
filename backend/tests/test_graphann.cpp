#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "../core/vector/VectorDB.h"
#include "../core/vector/VectorItem.h"
#include "../core/vector/SearchEngine.h"
#include "../core/vector/GraphANN.h"
#include "../core/vector/Similarity.h"
using namespace std;

Vector<float> generateRandomVector(int dim) {
    Vector<float> vec;
    for(int i = 0; i < dim; i++) {
        vec.push_back((float)rand() / RAND_MAX * 2.0f - 1.0f);
    }
    return vec;
}

Vector<float> normalizeVector(const Vector<float>& vec) {
    float magnitude = 0;
    for(int i = 0; i < vec.length(); i++) {
        magnitude += vec[i] * vec[i];
    }
    magnitude = sqrt(magnitude);
    
    Vector<float> normalized;
    for(int i = 0; i < vec.length(); i++) {
        normalized.push_back(vec[i] / magnitude);
    }
    return normalized;
}

Vector<float> createSimilarVector(const Vector<float>& base, float noise) {
    Vector<float> similar;
    for(int i = 0; i < base.length(); i++) {
        float val = base[i] + ((float)rand() / RAND_MAX * 2.0f - 1.0f) * noise;
        similar.push_back(val);
    }
    return normalizeVector(similar);
}

void testGraphConstruction() {
    cout << "\n=== Test 1: Graph Construction ===" << endl;
    
    VectorDB db;
    
    for(int i = 0; i < 10; i++) {
        db.insert(generateRandomVector(128));
    }
    
    GraphANN ann(&db, 3);
    ann.buildGraph();
    
    if(ann.getGraphSize() == 10) {
        cout << "PASS: Graph constructed with correct number of nodes" << endl;
    } else {
        cout << "FAIL: Graph size mismatch" << endl;
    }
}

void testGraphSearch() {
    cout << "\n=== Test 2: Graph Search vs Brute Force ===" << endl;
    
    VectorDB db;
    
    Vector<float> base = normalizeVector(generateRandomVector(128));
    db.insert(base);
    
    for(int i = 0; i < 50; i++) {
        if(i % 5 == 0) {
            db.insert(createSimilarVector(base, 0.2f));
        } else {
            db.insert(generateRandomVector(128));
        }
    }
    
    GraphANN ann(&db, 5);
    ann.buildGraph();
    
    SearchEngine bruteForce(&db);
    
    Vector<Pair<int, float>> annResults = ann.search(base, 5);
    Vector<Pair<int, float>> bfResults = bruteForce.search(base, 5);
    
    cout << "ANN Top 5:" << endl;
    for(int i = 0; i < annResults.length(); i++) {
        cout << "  ID: " << annResults[i].key << ", Sim: " << annResults[i].value << endl;
    }
    
    cout << "Brute Force Top 5:" << endl;
    for(int i = 0; i < bfResults.length(); i++) {
        cout << "  ID: " << bfResults[i].key << ", Sim: " << bfResults[i].value << endl;
    }
    
    int matches = 0;
    for(int i = 0; i < annResults.length(); i++) {
        for(int j = 0; j < bfResults.length(); j++) {
            if(annResults[i].key == bfResults[j].key) {
                matches++;
                break;
            }
        }
    }
    
    cout << "Overlap: " << matches << "/5 results match" << endl;
    
    if(matches >= 3) {
        cout << "PASS: GraphANN found most top results" << endl;
    } else {
        cout << "FAIL: Low overlap with ground truth" << endl;
    }
}

void testPerformance() {
    cout << "\n=== Test 3: Performance Comparison ===" << endl;
    
    VectorDB db;
    
    cout << "Inserting 500 vectors..." << endl;
    for(int i = 0; i < 500; i++) {
        db.insert(generateRandomVector(128));
    }
    
    Vector<float> query = generateRandomVector(128);
    
    cout << "\nBuilding graph..." << endl;
    clock_t buildStart = clock();
    GraphANN ann(&db, 10);
    ann.buildGraph();
    clock_t buildEnd = clock();
    double buildTime = (double)(buildEnd - buildStart) / CLOCKS_PER_SEC * 1000;
    cout << "Graph build time: " << buildTime << " ms" << endl;
    
    clock_t annStart = clock();
    Vector<Pair<int, float>> annResults = ann.search(query, 10);
    clock_t annEnd = clock();
    double annTime = (double)(annEnd - annStart) / CLOCKS_PER_SEC * 1000;
    
    SearchEngine bruteForce(&db);
    clock_t bfStart = clock();
    Vector<Pair<int, float>> bfResults = bruteForce.search(query, 10);
    clock_t bfEnd = clock();
    double bfTime = (double)(bfEnd - bfStart) / CLOCKS_PER_SEC * 1000;
    
    cout << "\nSearch Performance:" << endl;
    cout << "  GraphANN: " << annTime << " ms" << endl;
    cout << "  Brute Force: " << bfTime << " ms" << endl;
    cout << "  Speedup: " << (bfTime / annTime) << "x" << endl;
    
    if(annResults.length() == 10) {
        cout << "PASS: GraphANN returned correct number of results" << endl;
    } else {
        cout << "FAIL: Wrong number of results" << endl;
    }
}

void testEmptyDatabase() {
    cout << "\n=== Test 4: Empty Database ===" << endl;
    
    VectorDB db;
    GraphANN ann(&db, 5);
    
    Vector<float> query = generateRandomVector(128);
    Vector<Pair<int, float>> results = ann.search(query, 5);
    
    if(results.length() == 0) {
        cout << "PASS: Empty database handled correctly" << endl;
    } else {
        cout << "FAIL: Should return empty results" << endl;
    }
}

void testSmallDatabase() {
    cout << "\n=== Test 5: Small Database (k > db size) ===" << endl;
    
    VectorDB db;
    
    for(int i = 0; i < 3; i++) {
        db.insert(generateRandomVector(128));
    }
    
    GraphANN ann(&db, 2);
    ann.buildGraph();
    
    Vector<float> query = generateRandomVector(128);
    Vector<Pair<int, float>> results = ann.search(query, 10);
    
    cout << "Database size: 3, Requested k: 10, Got: " << results.length() << endl;
    
    if(results.length() <= 3) {
        cout << "PASS: Handled k > database size" << endl;
    } else {
        cout << "FAIL: Returned more results than database size" << endl;
    }
}

void testAccuracy() {
    cout << "\n=== Test 6: Accuracy Test ===" << endl;
    
    VectorDB db;
    
    Vector<float> query = normalizeVector(generateRandomVector(128));
    
    db.insert(query);
    for(int i = 0; i < 100; i++) {
        db.insert(generateRandomVector(128));
    }
    
    GraphANN ann(&db, 10);
    ann.buildGraph();
    
    Vector<Pair<int, float>> results = ann.search(query, 5);
    
    cout << "Searching for vector in database" << endl;
    cout << "Top result - ID: " << results[0].key << ", Similarity: " << results[0].value << endl;
    
    if(results[0].key == 0 && results[0].value > 0.99f) {
        cout << "PASS: Found exact match with highest similarity" << endl;
    } else {
        cout << "FAIL: Should find exact match key" << endl;
    }
}

int main() {
    srand(time(0));
    
    cout << "====================================" << endl;
    cout << "GraphANN Test Suite" << endl;
    cout << "====================================" << endl;
    
    testGraphConstruction();
    testGraphSearch();
    testEmptyDatabase();
    testSmallDatabase();
    testAccuracy();
    testPerformance();
    
    cout << "\n====================================" << endl;
    cout << "All tests completed!" << endl;
    cout << "====================================" << endl;
    
    return 0;
}
