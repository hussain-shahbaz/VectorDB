#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "../core/vector/VectorDB.h"
#include "../core/vector/VectorItem.h"
#include "../core/vector/SearchEngine.h"
#include "../core/vector/Similarity.h"

using namespace std;

Vector<float> randomVector(int dim) {
    Vector<float> v;
    for (int i = 0; i < dim; i++) {
        v.push_back(((float)(rand() % 20000) / 10000.0f) - 1.0f);
    }
    return v;
}

void testBasicRoundTrip() {
    cout << "=== Test 1: Basic Save/Load Round Trip ===" << endl;
    srand(1);

    VectorDB db;
    int n = 50, dim = 32;
    for (int i = 0; i < n; i++) db.insert(randomVector(dim));

    const char* path = "/tmp/vectordb_test_basic.vdb";
    bool saved = db.save(path);
    cout << "save(): " << (saved ? "OK" : "FAILED") << endl;

    VectorDB loaded;
    bool ok = loaded.load(path);
    cout << "load(): " << (ok ? "OK" : "FAILED") << endl;

    cout << "Original size: " << db.size() << ", loaded size: " << loaded.size() << " (expected equal)" << endl;

    bool allMatch = (db.size() == loaded.size());
    for (int i = 0; allMatch && i < db.size(); i++) {
        VectorItem* a = db.at(i);
        VectorItem* b = loaded.get(a->id);
        if (!b || b->embedding.length() != a->embedding.length()) { allMatch = false; break; }
        for (int d = 0; d < a->embedding.length(); d++) {
            if (a->embedding[d] != b->embedding[d]) { allMatch = false; break; }
        }
    }

    cout << (saved && ok && allMatch ? "PASS" : "FAIL")
         << ": every vector's embedding survives the round trip bit-for-bit (Huffman is lossless)" << endl;

    cout << endl;
}

void testSearchResultsIdenticalAfterReload() {
    cout << "=== Test 2: Search Results Identical After Reload ===" << endl;
    srand(2);

    VectorDB db;
    int n = 100, dim = 16;
    for (int i = 0; i < n; i++) db.insert(randomVector(dim));

    Vector<float> query = randomVector(dim);
    SearchEngine originalEngine(&db);
    Vector<Pair<int, float>> before = originalEngine.search(query, 5);

    const char* path = "/tmp/vectordb_test_search.vdb";
    db.save(path);

    VectorDB loaded;
    loaded.load(path);
    SearchEngine loadedEngine(&loaded);
    Vector<Pair<int, float>> after = loadedEngine.search(query, 5);

    bool match = before.length() == after.length();
    for (int i = 0; match && i < before.length(); i++) {
        if (before[i].key != after[i].key || before[i].value != after[i].value) match = false;
    }

    cout << "Top-5 before save: ";
    for (int i = 0; i < before.length(); i++) cout << before[i].key << " ";
    cout << endl << "Top-5 after load:  ";
    for (int i = 0; i < after.length(); i++) cout << after[i].key << " ";
    cout << endl;

    cout << (match ? "PASS" : "FAIL") << ": identical query against the loaded DB returns identical results" << endl;

    cout << endl;
}

void testDeletedItemsCompactedOnSave() {
    cout << "=== Test 3: Deleted Items Are Compacted on Save ===" << endl;
    srand(3);

    VectorDB db;
    int ids[10];
    for (int i = 0; i < 10; i++) ids[i] = db.insert(randomVector(8));

    db.remove(ids[3]);
    db.remove(ids[7]);
    cout << "Deleted 2 of 10 vectors. activeSize(): " << db.activeSize() << " (expected: 8)" << endl;

    const char* path = "/tmp/vectordb_test_compact.vdb";
    db.save(path);

    VectorDB loaded;
    loaded.load(path);

    cout << "Loaded size: " << loaded.size() << " (expected: 8, deleted ones dropped)" << endl;
    bool deletedGone = (loaded.get(ids[3]) == nullptr && loaded.get(ids[7]) == nullptr);
    bool othersPresent = true;
    for (int i = 0; i < 10; i++) {
        if (i == 3 || i == 7) continue;
        if (loaded.get(ids[i]) == nullptr) othersPresent = false;
    }

    cout << (loaded.size() == 8 && deletedGone && othersPresent ? "PASS" : "FAIL")
         << ": save() compacts away soft-deleted items, everything else survives" << endl;

    cout << endl;
}

void testEmptyDatabaseRoundTrip() {
    cout << "=== Test 4: Empty Database Round Trip ===" << endl;

    VectorDB db;
    const char* path = "/tmp/vectordb_test_empty.vdb";
    bool saved = db.save(path);

    VectorDB loaded;
    bool ok = loaded.load(path);

    cout << "save(): " << (saved ? "OK" : "FAILED") << ", load(): " << (ok ? "OK" : "FAILED")
         << ", loaded size: " << loaded.size() << " (expected: 0)" << endl;
    cout << (saved && ok && loaded.size() == 0 ? "PASS" : "FAIL") << ": empty database round-trips without crashing" << endl;

    cout << endl;
}

void testLoadNonexistentOrCorruptFile() {
    cout << "=== Test 5: Loading a Bad File Fails Gracefully ===" << endl;

    VectorDB db1;
    bool okMissing = db1.load("/tmp/this_file_should_not_exist_vectordb.vdb");
    cout << (!okMissing ? "PASS" : "FAIL") << ": loading a nonexistent file returns false" << endl;

    ofstream junk("/tmp/vectordb_test_junk.vdb", ios::binary);
    junk << "not a real vectordb file";
    junk.close();

    VectorDB db2;
    bool okJunk = db2.load("/tmp/vectordb_test_junk.vdb");
    cout << (!okJunk ? "PASS" : "FAIL") << ": loading a file with a bad magic number returns false, doesn't crash" << endl;

    cout << endl;
}

void testNextIdContinuesAfterLoad() {
    cout << "=== Test 6: nextId Continues Correctly After Load ===" << endl;
    srand(4);

    VectorDB db;
    db.insert(randomVector(4));
    db.insert(randomVector(4));
    int lastId = db.insert(randomVector(4)); // id 2

    const char* path = "/tmp/vectordb_test_nextid.vdb";
    db.save(path);

    VectorDB loaded;
    loaded.load(path);
    int newId = loaded.insert(randomVector(4));

    cout << "Last id before save: " << lastId << ", first id inserted after load: " << newId
         << " (expected: " << (lastId + 1) << ", no collision)" << endl;
    cout << (newId == lastId + 1 ? "PASS" : "FAIL") << ": inserting after load doesn't reuse an existing id" << endl;

    cout << endl;
}

int main() {
    cout << "======================================" << endl;
    cout << "    VectorDB Persistence Test Suite" << endl;
    cout << "======================================" << endl;
    cout << endl;

    testBasicRoundTrip();
    testSearchResultsIdenticalAfterReload();
    testDeletedItemsCompactedOnSave();
    testEmptyDatabaseRoundTrip();
    testLoadNonexistentOrCorruptFile();
    testNextIdContinuesAfterLoad();

    cout << "======================================" << endl;
    cout << "    All Tests Complete" << endl;
    cout << "======================================" << endl;

    return 0;
}
