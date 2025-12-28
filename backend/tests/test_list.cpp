#include <iostream>
#include "../backend/dataStructures/List.h"

using namespace std;

void testBasicOperations() {
    cout << "=== Test 1: Basic Insert and Access ===" << endl;
    List<int> list;
    
    list.insertAtEnd(10);
    list.insertAtEnd(20);
    list.insertAtEnd(30);
    
    cout << "Inserted at end: 10, 20, 30" << endl;
    cout << "Size: " << list.size() << " (expected: 3)" << endl;
    
    cout << "List: ";
    list.print();
    
    cout << endl;
}

void testInsertAtFront() {
    cout << "=== Test 2: Insert at Front ===" << endl;
    List<int> list;
    
    list.insertAtFront(30);
    list.insertAtFront(20);
    list.insertAtFront(10);
    
    cout << "Inserted at front: 30, 20, 10" << endl;
    cout << "Size: " << list.size() << " (expected: 3)" << endl;
    
    cout << "List (should be 10 20 30): ";
    list.print();
    
    cout << endl;
}

void testSearch() {
    cout << "=== Test 3: Search ===" << endl;
    List<int> list;
    
    list.insertAtEnd(10);
    list.insertAtEnd(20);
    list.insertAtEnd(30);
    list.insertAtEnd(40);
    
    cout << "List: ";
    list.print();
    
    bool found20 = list.search(20);
    bool found99 = list.search(99);
    
    cout << "Search 20: " << (found20 ? "FOUND" : "NOT FOUND") << endl;
    cout << "Search 99: " << (found99 ? "FOUND" : "NOT FOUND") << endl;
    
    cout << endl;
}

void testRemove() {
    cout << "=== Test 4: Remove ===" << endl;
    List<int> list;
    
    list.insertAtEnd(10);
    list.insertAtEnd(20);
    list.insertAtEnd(30);
    list.insertAtEnd(40);
    
    cout << "Before remove: ";
    list.print();
    
    bool removed = list.remove(20);
    cout << "Remove 20: " << (removed ? "SUCCESS" : "FAILED") << endl;
    
    cout << "After remove: ";
    list.print();
    cout << "Size: " << list.size() << " (expected: 3)" << endl;
    
    cout << endl;
}

void testRemoveFirst() {
    cout << "=== Test 5: Remove First Element ===" << endl;
    List<int> list;
    
    list.insertAtEnd(10);
    list.insertAtEnd(20);
    list.insertAtEnd(30);
    
    cout << "Before: ";
    list.print();
    
    list.remove(10);
    
    cout << "After removing first (10): ";
    list.print();
    
    cout << endl;
}

void testRemoveLast() {
    cout << "=== Test 6: Remove Last Element ===" << endl;
    List<int> list;
    
    list.insertAtEnd(10);
    list.insertAtEnd(20);
    list.insertAtEnd(30);
    
    cout << "Before: ";
    list.print();
    
    list.remove(30);
    
    cout << "After removing last (30): ";
    list.print();
    
    cout << endl;
}

void testEmpty() {
    cout << "=== Test 7: Empty List ===" << endl;
    List<int> list;
    
    cout << "Size of empty list: " << list.size() << " (expected: 0)" << endl;
    
    bool found = list.search(42);
    cout << "Search in empty list: " << (found ? "FOUND (ERROR)" : "NOT FOUND (correct)") << endl;
    
    bool removed = list.remove(42);
    cout << "Remove from empty list: " << (removed ? "SUCCESS (ERROR)" : "FAILED (correct)") << endl;
    
    cout << endl;
}

void testLargeList() {
    cout << "=== Test 8: Large List ===" << endl;
    List<int> list;
    
    int n = 50;
    for(int i = 0; i < n; i++){
        list.insertAtEnd(i);
    }
    
    cout << "Inserted " << n << " elements" << endl;
    cout << "Size: " << list.size() << endl;
    
    bool found25 = list.search(25);
    bool found99 = list.search(99);
    
    cout << "Search 25: " << (found25 ? "FOUND" : "NOT FOUND") << endl;
    cout << "Search 99: " << (found99 ? "FOUND" : "NOT FOUND") << endl;
    
    list.remove(25);
    cout << "After removing 25 - Size: " << list.size() << " (expected: " << (n-1) << ")" << endl;
    
    cout << endl;
}

int main() {
    cout << "======================================" << endl;
    cout << "    Linked List Test Suite" << endl;
    cout << "======================================" << endl;
    cout << endl;
    
    testBasicOperations();
    testInsertAtFront();
    testSearch();
    testRemove();
    testRemoveFirst();
    testRemoveLast();
    testEmpty();
    testLargeList();
    
    cout << "======================================" << endl;
    cout << "    All Tests Complete" << endl;
    cout << "======================================" << endl;
    
    return 0;
}
