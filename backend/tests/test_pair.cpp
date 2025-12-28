#include <iostream>
#include "../dataStructures/Pair.h"

using namespace std;

void testBasicPair() {
    cout << "=== Test 1: Basic Pair Creation ===" << endl;
    Pair<int, int> p1(10, 100);
    
    cout << "Pair: key=" << p1.key << ", value=" << p1.value << endl;
    cout << "Expected: key=10, value=100" << endl;
    
    cout << endl;
}

void testComparison() {
    cout << "=== Test 2: Comparison Operators ===" << endl;
    Pair<int, int> p1(10, 100);
    Pair<int, int> p2(20, 200);
    Pair<int, int> p3(10, 999);
    
    cout << "p1: key=" << p1.key << ", value=" << p1.value << endl;
    cout << "p2: key=" << p2.key << ", value=" << p2.value << endl;
    cout << "p3: key=" << p3.key << ", value=" << p3.value << endl;
    
    cout << "p1 < p2: " << (p1 < p2) << " (expected: 1)" << endl;
    cout << "p2 < p1: " << (p2 < p1) << " (expected: 0)" << endl;
    cout << "p1 == p3: " << (p1 == p3) << " (expected: 1, compares key only)" << endl;
    
    cout << endl;
}

void testKeyOnlyComparison() {
    cout << "=== Test 3: Key-Only Comparison ===" << endl;
    Pair<int, long long> p1(5, 1000LL);
    Pair<int, long long> p2(5, 9999LL);
    
    cout << "p1: key=" << p1.key << ", value=" << p1.value << endl;
    cout << "p2: key=" << p2.key << ", value=" << p2.value << endl;
    
    cout << "Same key, different values" << endl;
    cout << "p1 == p2: " << (p1 == p2) << " (expected: 1)" << endl;
    cout << "p1 < p2: " << (p1 < p2) << " (expected: 0)" << endl;
    
    cout << endl;
}

void testFloatPair() {
    cout << "=== Test 4: Float Key Pair ===" << endl;
    Pair<float, int> p1(3.14f, 100);
    Pair<float, int> p2(2.71f, 200);
    
    cout << "p1: key=" << p1.key << ", value=" << p1.value << endl;
    cout << "p2: key=" << p2.key << ", value=" << p2.value << endl;
    
    cout << "p1 < p2: " << (p1 < p2) << " (expected: 0)" << endl;
    cout << "p2 < p1: " << (p2 < p1) << " (expected: 1)" << endl;
    
    cout << endl;
}

void testStringValuePair() {
    cout << "=== Test 5: String Value Pair ===" << endl;
    Pair<int, const char*> p1(1, "apple");
    Pair<int, const char*> p2(2, "banana");
    
    cout << "p1: key=" << p1.key << ", value=" << p1.value << endl;
    cout << "p2: key=" << p2.key << ", value=" << p2.value << endl;
    
    cout << "p1 < p2: " << (p1 < p2) << " (expected: 1)" << endl;
    cout << "p1 == p2: " << (p1 == p2) << " (expected: 0)" << endl;
    
    cout << endl;
}

void testSortingBehavior() {
    cout << "=== Test 6: Sorting Behavior ===" << endl;
    Pair<int, int> pairs[5] = {
        Pair<int, int>(50, 1),
        Pair<int, int>(20, 2),
        Pair<int, int>(80, 3),
        Pair<int, int>(10, 4),
        Pair<int, int>(40, 5)
    };
    
    cout << "Before sorting (by key):" << endl;
    for(int i = 0; i < 5; i++){
        cout << "  key=" << pairs[i].key << ", value=" << pairs[i].value << endl;
    }
    
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < 4 - i; j++){
            if(pairs[j+1] < pairs[j]){
                Pair<int, int> temp = pairs[j];
                pairs[j] = pairs[j+1];
                pairs[j+1] = temp;
            }
        }
    }
    
    cout << "After sorting:" << endl;
    for(int i = 0; i < 5; i++){
        cout << "  key=" << pairs[i].key << ", value=" << pairs[i].value << endl;
    }
    cout << "Expected order: 10, 20, 40, 50, 80" << endl;
    
    cout << endl;
}

void testIDOffsetPair() {
    cout << "=== Test 7: ID-Offset Pair (Real Use Case) ===" << endl;
    Pair<int, long long> entry1(101, 4096LL);
    Pair<int, long long> entry2(102, 8192LL);
    Pair<int, long long> searchKey(101, 0LL);
    
    cout << "entry1: ID=" << entry1.key << ", offset=" << entry1.value << endl;
    cout << "entry2: ID=" << entry2.key << ", offset=" << entry2.value << endl;
    cout << "searchKey: ID=" << searchKey.key << ", offset=" << searchKey.value << endl;
    
    cout << "searchKey == entry1: " << (searchKey == entry1) << " (expected: 1)" << endl;
    cout << "searchKey == entry2: " << (searchKey == entry2) << " (expected: 0)" << endl;
    
    cout << "This allows searching by ID without knowing offset" << endl;
    
    cout << endl;
}

int main() {
    cout << "======================================" << endl;
    cout << "    Pair Test Suite" << endl;
    cout << "======================================" << endl;
    cout << endl;
    
    testBasicPair();
    testComparison();
    testKeyOnlyComparison();
    testFloatPair();
    testStringValuePair();
    testSortingBehavior();
    testIDOffsetPair();
    
    cout << "======================================" << endl;
    cout << "    All Tests Complete" << endl;
    cout << "======================================" << endl;
    
    return 0;
}
