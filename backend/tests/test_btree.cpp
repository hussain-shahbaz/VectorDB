#include <iostream>
#include "../dataStructures/BTree.h"
#include "../dataStructures/Pair.h"

using namespace std;

void testBasicInsertSearch() {
    cout << "=== Test 1: Basic Insert and Search ===" << endl;
    BTree<int> tree(3);
    
    tree.insert(10);
    tree.insert(20);
    tree.insert(30);
    tree.insert(5);
    tree.insert(15);
    
    cout << "Inserted: 10, 20, 30, 5, 15" << endl;
    
    int* found10 = tree.search(10);
    int* found20 = tree.search(20);
    int* found99 = tree.search(99);
    
    cout << "Search 10: " << (found10 ? "FOUND" : "NOT FOUND") << endl;
    cout << "Search 20: " << (found20 ? "FOUND" : "NOT FOUND") << endl;
    cout << "Search 99: " << (found99 ? "FOUND" : "NOT FOUND") << endl;
    
    cout << endl;
}

void testPairBTree() {
    cout << "=== Test 2: BTree with Pairs (ID -> Offset) ===" << endl;
    BTree<Pair<int, long long>> tree(3);
    
    tree.insert(Pair<int, long long>(1, 1000LL));
    tree.insert(Pair<int, long long>(2, 2000LL));
    tree.insert(Pair<int, long long>(3, 3000LL));
    tree.insert(Pair<int, long long>(4, 4000LL));
    tree.insert(Pair<int, long long>(5, 5000LL));
    
    cout << "Inserted 5 ID-offset pairs" << endl;
    
    Pair<int, long long>* result = tree.search(Pair<int, long long>(3, 0LL));
    if(result){
        cout << "Found ID 3: offset = " << result->value << " (expected: 3000)" << endl;
    } else {
        cout << "ID 3 not found" << endl;
    }
    
    Pair<int, long long>* result2 = tree.search(Pair<int, long long>(5, 0LL));
    if(result2){
        cout << "Found ID 5: offset = " << result2->value << " (expected: 5000)" << endl;
    }
    
    cout << endl;
}

void testLargeInsertions() {
    cout << "=== Test 3: Large Number of Insertions ===" << endl;
    BTree<int> tree(3);
    
    int n = 50;
    for(int i = 0; i < n; i++){
        tree.insert(i);
    }
    
    cout << "Inserted " << n << " values (0 to " << (n-1) << ")" << endl;
    
    bool allFound = true;
    for(int i = 0; i < n; i++){
        int* result = tree.search(i);
        if(!result){
            allFound = false;
            cout << "ERROR: Value " << i << " not found!" << endl;
        }
    }
    
    cout << "All values found: " << (allFound ? "YES" : "NO") << endl;
    
    int* notExists = tree.search(999);
    cout << "Search for 999: " << (notExists ? "FOUND (ERROR)" : "NOT FOUND (correct)") << endl;
    
    cout << endl;
}

void testDuplicateHandling() {
    cout << "=== Test 4: Duplicate Handling ===" << endl;
    BTree<int> tree(3);
    
    tree.insert(10);
    tree.insert(20);
    tree.insert(10);
    
    cout << "Inserted: 10, 20, 10 (duplicate)" << endl;
    
    int* found = tree.search(10);
    cout << "Search for 10: " << (found ? "FOUND" : "NOT FOUND") << endl;
    
    cout << endl;
}

void testSequentialInsert() {
    cout << "=== Test 5: Sequential Insert ===" << endl;
    BTree<int> tree(5);
    
    for(int i = 1; i <= 20; i++){
        tree.insert(i);
    }
    
    cout << "Inserted sequential values 1-20" << endl;
    
    int* first = tree.search(1);
    int* middle = tree.search(10);
    int* last = tree.search(20);
    
    cout << "Search 1: " << (first ? "FOUND" : "NOT FOUND") << endl;
    cout << "Search 10: " << (middle ? "FOUND" : "NOT FOUND") << endl;
    cout << "Search 20: " << (last ? "FOUND" : "NOT FOUND") << endl;
    
    cout << endl;
}

void testReverseInsert() {
    cout << "=== Test 6: Reverse Order Insert ===" << endl;
    BTree<int> tree(5);
    
    for(int i = 20; i >= 1; i--){
        tree.insert(i);
    }
    
    cout << "Inserted values 20 down to 1" << endl;
    
    int* first = tree.search(1);
    int* middle = tree.search(10);
    int* last = tree.search(20);
    
    cout << "Search 1: " << (first ? "FOUND" : "NOT FOUND") << endl;
    cout << "Search 10: " << (middle ? "FOUND" : "NOT FOUND") << endl;
    cout << "Search 20: " << (last ? "FOUND" : "NOT FOUND") << endl;
    
    cout << endl;
}

void testDifferentDegrees() {
    cout << "=== Test 7: Different Minimum Degrees ===" << endl;
    
    for(int degree = 2; degree <= 5; degree++){
        BTree<int> tree(degree);
        
        for(int i = 0; i < 15; i++){
            tree.insert(i * 10);
        }
        
        int* result = tree.search(70);
        cout << "Degree " << degree << " - Search 70: " << (result ? "FOUND" : "NOT FOUND") << endl;
    }
    
    cout << endl;
}

int main() {
    cout << "======================================" << endl;
    cout << "    BTree Test Suite" << endl;
    cout << "======================================" << endl;
    cout << endl;
    
    testBasicInsertSearch();
    testPairBTree();
    testLargeInsertions();
    testDuplicateHandling();
    testSequentialInsert();
    testReverseInsert();
    testDifferentDegrees();
    
    cout << "======================================" << endl;
    cout << "    All Tests Complete" << endl;
    cout << "======================================" << endl;
    
    return 0;
}
