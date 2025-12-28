#include <iostream>
#include "../dataStructures/HashMap.h"

using namespace std;

void testBasicOperations() {
    cout << "=== Test 1: Basic Insert and Find ===" << endl;
    HashMap<int, int> map(7);
    
    map.insert(1, 100);
    map.insert(2, 200);
    map.insert(3, 300);
    
    int* val1 = map.find(1);
    int* val2 = map.find(2);
    int* val3 = map.find(3);
    
    cout << "Find 1: " << (val1 ? *val1 : -1) << " (expected: 100)" << endl;
    cout << "Find 2: " << (val2 ? *val2 : -1) << " (expected: 200)" << endl;
    cout << "Find 3: " << (val3 ? *val3 : -1) << " (expected: 300)" << endl;
    cout << "Size: " << map.size << " (expected: 3)" << endl;
    cout << endl;
}

void testUpdate() {
    cout << "=== Test 2: Update Existing Key ===" << endl;
    HashMap<int, int> map(7);
    
    map.insert(1, 100);
    cout << "Before update: " << *map.find(1) << endl;
    
    map.insert(1, 999);
    cout << "After update: " << *map.find(1) << " (expected: 999)" << endl;
    cout << "Size: " << map.size << " (expected: 1)" << endl;
    cout << endl;
}

void testCollisions() {
    cout << "=== Test 3: Handle Collisions ===" << endl;
    HashMap<int, int> map(7);
    
    map.insert(1, 111);
    map.insert(8, 888);
    map.insert(15, 1515);
    
    int* v1 = map.find(1);
    int* v8 = map.find(8);
    int* v15 = map.find(15);
    
    cout << "Find 1: " << (v1 ? *v1 : -1) << endl;
    cout << "Find 8: " << (v8 ? *v8 : -1) << endl;
    cout << "Find 15: " << (v15 ? *v15 : -1) << endl;
    cout << endl;
}

void testRehash() {
    cout << "=== Test 4: Rehashing ===" << endl;
    HashMap<int, int> map(4);
    
    cout << "Initial capacity: " << map.capacity << endl;
    
    for(int i = 0; i < 10; i++){
        map.insert(i, i * 100);
    }
    
    cout << "After 10 inserts:" << endl;
    cout << "  Capacity: " << map.capacity << " (should have grown)" << endl;
    cout << "  Size: " << map.size << endl;
    
    for(int i = 0; i < 10; i++){
        int* val = map.find(i);
        if(val == nullptr || *val != i * 100){
            cout << "ERROR: Key " << i << " not found or wrong value!" << endl;
        }
    }
    cout << "All keys found correctly after rehash" << endl;
    cout << endl;
}

void testRemove() {
    cout << "=== Test 5: Remove Operation ===" << endl;
    HashMap<int, int> map(7);
    
    map.insert(1, 111);
    map.insert(2, 222);
    map.insert(3, 333);
    
    cout << "Before remove - Size: " << map.size << endl;
    
    bool removed = map.remove(2);
    cout << "Remove key 2: " << (removed ? "SUCCESS" : "FAILED") << endl;
    cout << "After remove - Size: " << map.size << " (expected: 2)" << endl;
    
    int* val = map.find(2);
    cout << "Find removed key 2: " << (val ? "FOUND (ERROR)" : "NOT FOUND (correct)") << endl;
    
    int* val1 = map.find(1);
    int* val3 = map.find(3);
    cout << "Key 1 still exists: " << (val1 ? "YES" : "NO") << endl;
    cout << "Key 3 still exists: " << (val3 ? "YES" : "NO") << endl;
    cout << endl;
}

void testNotFound() {
    cout << "=== Test 6: Not Found Cases ===" << endl;
    HashMap<int, int> map(7);
    
    map.insert(1, 100);
    
    int* val = map.find(999);
    cout << "Find non-existent key 999: " << (val ? "FOUND (ERROR)" : "NOT FOUND (correct)") << endl;
    
    bool removed = map.remove(999);
    cout << "Remove non-existent key: " << (removed ? "SUCCESS (ERROR)" : "FAILED (correct)") << endl;
    cout << endl;
}

void testStringKeys() {
    cout << "=== Test 7: String Keys ===" << endl;
    HashMap<int, const char*> map(7);
    
    map.insert(1, "apple");
    map.insert(2, "banana");
    map.insert(3, "cherry");
    
    const char** fruit = map.find(2);
    cout << "Find key 2: " << (fruit ? *fruit : "NOT FOUND") << " (expected: banana)" << endl;
    cout << endl;
}

int main() {
    cout << "======================================" << endl;
    cout << "    HashMap Test Suite" << endl;
    cout << "======================================" << endl;
    cout << endl;
    
    testBasicOperations();
    testUpdate();
    testCollisions();
    testRehash();
    testRemove();
    testNotFound();
    testStringKeys();
    
    cout << "======================================" << endl;
    cout << "    All Tests Complete" << endl;
    cout << "======================================" << endl;
    
    return 0;
}
