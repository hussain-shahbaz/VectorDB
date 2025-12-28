#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "../core/vector/VectorDB.h"
#include "../core/vector/VectorItem.h"
#include "../core/vector/SearchEngine.h"
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

void testBasicSearch() {
    cout << "\n=== Test 1: Basic Search ===" << endl;
    
    VectorDB db;
    SearchEngine engine(&db);
    
    Vector<float> v1 = generateRandomVector(128);
    Vector<float> v2 = generateRandomVector(128);
    Vector<float> v3 = generateRandomVector(128);
    
    db.insert(v1);
    db.insert(v2);
    db.insert(v3);
    
    Vector<Pair<int, float>> results = engine.search(v1, 3);
    
    cout << "Inserted 3 vectors, searching for key vector" << endl;
    cout << "Top 3 results:" << endl;
    for(int i = 0; i < results.length(); i++) {
        cout << "ID: " << results[i].key << ", Similarity: " << results[i].value << endl;
    }
    
    if(results[0].key == 0 && results[0].value > 0.99f) {
        cout << "PASS: Query vector found itself with highest similarity" << endl;
    } else {
        cout << "FAIL: Query vector should match itself" << endl;
    }
}

void testTopKRetrieval() {
    cout << "\n=== Test 2: Top-K Retrieval ===" << endl;
    
    VectorDB db;
    SearchEngine engine(&db);
    
    Vector<float> base = normalizeVector(generateRandomVector(128));
    
    db.insert(base);
    db.insert(createSimilarVector(base, 0.1f));
    db.insert(createSimilarVector(base, 0.2f));
    db.insert(createSimilarVector(base, 0.5f));
    db.insert(createSimilarVector(base, 0.8f));
    db.insert(generateRandomVector(128));
    db.insert(generateRandomVector(128));
    
    Vector<Pair<int, float>> results = engine.search(base, 3);
    
    cout << "Inserted 7 vectors (5 similar + 2 random)" << endl;
    cout << "Top 3 results:" << endl;
    
    bool passed = true;
    for(int i = 0; i < results.length(); i++) {
        cout << "Rank " << i+1 << " - ID: " << results[i].key 
             << ", Similarity: " << results[i].value << endl;
        
        if(i > 0 && results[i].value > results[i-1].value) {
            passed = false;
        }
    }
    
    if(passed && results.length() == 3) {
        cout << "PASS: Results ranked in descending order" << endl;
    } else {
        cout << "FAIL: Results not properly ranked" << endl;
    }
}

void testLargeDatabase() {
    cout << "\n=== Test 3: Large Database Search ===" << endl;
    
    VectorDB db;
    SearchEngine engine(&db);
    
    Vector<float> query = normalizeVector(generateRandomVector(128));
    
    cout << "Inserting 1000 vectors..." << endl;
    for(int i = 0; i < 1000; i++) {
        if(i % 10 == 0) {
            db.insert(createSimilarVector(query, 0.3f));
        } else {
            db.insert(generateRandomVector(128));
        }
    }
    
    clock_t start = clock();
    Vector<Pair<int, float>> results = engine.search(query, 10);
    clock_t end = clock();
    
    double timeTaken = (double)(end - start) / CLOCKS_PER_SEC * 1000;
    
    cout << "Search completed in " << timeTaken << " ms" << endl;
    cout << "Top 10 results:" << endl;
    
    int similarFound = 0;
    for(int i = 0; i < results.length(); i++) {
        cout << "Rank " << i+1 << " - ID: " << results[i].key 
             << ", Similarity: " << results[i].value << endl;
        
        if(results[i].key % 10 == 0) {
            similarFound++;
        }
    }
    
    cout << "Found " << similarFound << "/10 intentionally similar vectors in top 10" << endl;
    
    if(results.length() == 10 && similarFound >= 5) {
        cout << "PASS: Found majority of similar vectors" << endl;
    } else {
        cout << "FAIL: Should find similar vectors in top results" << endl;
    }
}

void testEmptyDatabase() {
    cout << "\n=== Test 4: Empty Database Search ===" << endl;
    
    VectorDB db;
    SearchEngine engine(&db);
    
    Vector<float> query = generateRandomVector(128);
    Vector<Pair<int, float>> results = engine.search(query, 5);
    
    if(results.length() == 0) {
        cout << "PASS: Empty database returns no results" << endl;
    } else {
        cout << "FAIL: Should return empty results" << endl;
    }
}

void testSingleVector() {
    cout << "\n=== Test 5: Single Vector Database ===" << endl;
    
    VectorDB db;
    SearchEngine engine(&db);
    
    Vector<float> v1 = generateRandomVector(128);
    db.insert(v1);
    
    Vector<Pair<int, float>> results = engine.search(v1, 10);
    
    if(results.length() == 1 && results[0].key == 0) {
        cout << "PASS: Single vector database returns that vector" << endl;
    } else {
        cout << "FAIL: Should return the only vector" << endl;
    }
}

void testSimilarityScores() {
    cout << "\n=== Test 6: Similarity Score Validation ===" << endl;
    
    VectorDB db;
    SearchEngine engine(&db);
    
    Vector<float> v1 = normalizeVector(generateRandomVector(128));
    Vector<float> v2 = normalizeVector(generateRandomVector(128));
    
    db.insert(v1);
    db.insert(v2);
    
    Vector<Pair<int, float>> results = engine.search(v1, 2);
    
    cout << "Self-similarity: " << results[0].value << endl;
    cout << "Random similarity: " << results[1].value << endl;
    
    if(results[0].value > 0.99f && results[0].value <= 1.0f) {
        cout << "PASS: Self-similarity is ~1.0" << endl;
    } else {
        cout << "FAIL: Self-similarity should be close to 1.0" << endl;
    }
    
    if(results[1].value >= -1.0f && results[1].value <= 1.0f) {
        cout << "PASS: Cosine similarity in valid range [-1, 1]" << endl;
    } else {
        cout << "FAIL: Invalid similarity score" << endl;
    }
}

int main() {
    srand(time(0));
    
    cout << "====================================" << endl;
    cout << "Search Engine Test Suite (128D)" << endl;
    cout << "====================================" << endl;
    
    testBasicSearch();
    testTopKRetrieval();
    testEmptyDatabase();
    testSingleVector();
    testSimilarityScores();
    testLargeDatabase();
    
    cout << "\n====================================" << endl;
    cout << "All tests completed!" << endl;
    cout << "====================================" << endl;
    
    return 0;
}