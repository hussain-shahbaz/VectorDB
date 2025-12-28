#include <iostream>
#include "../dataStructures/Heap.h"

using namespace std;

void testMinHeapBasic() {
    cout << "=== Test 1: MinHeap Basic Operations ===" << endl;
    MinHeap<int> minHeap(20);
    
    minHeap.push(5);
    minHeap.push(3);
    minHeap.push(7);
    minHeap.push(1);
    minHeap.push(9);
    
    cout << "Inserted: 5, 3, 7, 1, 9" << endl;
    cout << "Min heap top: " << minHeap.top() << " (expected: 1)" << endl;
    cout << "Size: " << minHeap.size() << " (expected: 5)" << endl;
    
    minHeap.pop();
    cout << "After pop: " << minHeap.top() << " (expected: 3)" << endl;
    cout << "Size: " << minHeap.size() << " (expected: 4)" << endl;
    cout << endl;
}

void testMaxHeapBasic() {
    cout << "=== Test 2: MaxHeap Basic Operations ===" << endl;
    MaxHeap<int> maxHeap(20);
    
    maxHeap.push(5);
    maxHeap.push(3);
    maxHeap.push(7);
    maxHeap.push(1);
    maxHeap.push(9);
    
    cout << "Inserted: 5, 3, 7, 1, 9" << endl;
    cout << "Max heap top: " << maxHeap.top() << " (expected: 9)" << endl;
    cout << "Size: " << maxHeap.size() << " (expected: 5)" << endl;
    
    maxHeap.pop();
    cout << "After pop: " << maxHeap.top() << " (expected: 7)" << endl;
    cout << "Size: " << maxHeap.size() << " (expected: 4)" << endl;
    cout << endl;
}

void testMinHeapOrdering() {
    cout << "=== Test 3: MinHeap Complete Ordering ===" << endl;
    MinHeap<int> minHeap(20);
    
    int values[] = {15, 10, 20, 8, 25, 5, 12};
    int n = 7;
    
    cout << "Inserting: ";
    for(int i = 0; i < n; i++){
        cout << values[i] << " ";
        minHeap.push(values[i]);
    }
    cout << endl;
    
    cout << "Extracting in order: ";
    int expected[] = {5, 8, 10, 12, 15, 20, 25};
    bool correct = true;
    
    for(int i = 0; i < n; i++){
        int val = minHeap.top();
        cout << val << " ";
        if(val != expected[i]) correct = false;
        minHeap.pop();
    }
    cout << endl;
    cout << "Order correct: " << (correct ? "YES" : "NO") << endl;
    cout << endl;
}

void testMaxHeapOrdering() {
    cout << "=== Test 4: MaxHeap Complete Ordering ===" << endl;
    MaxHeap<int> maxHeap(20);
    
    int values[] = {15, 10, 20, 8, 25, 5, 12};
    int n = 7;
    
    cout << "Inserting: ";
    for(int i = 0; i < n; i++){
        cout << values[i] << " ";
        maxHeap.push(values[i]);
    }
    cout << endl;
    
    cout << "Extracting in order: ";
    int expected[] = {25, 20, 15, 12, 10, 8, 5};
    bool correct = true;
    
    for(int i = 0; i < n; i++){
        int val = maxHeap.top();
        cout << val << " ";
        if(val != expected[i]) correct = false;
        maxHeap.pop();
    }
    cout << endl;
    cout << "Order correct: " << (correct ? "YES" : "NO") << endl;
    cout << endl;
}

void testHeapWithDuplicates() {
    cout << "=== Test 5: Heap with Duplicates ===" << endl;
    MinHeap<int> minHeap(20);
    
    minHeap.push(5);
    minHeap.push(3);
    minHeap.push(5);
    minHeap.push(3);
    minHeap.push(1);
    
    cout << "Inserted: 5, 3, 5, 3, 1" << endl;
    cout << "Top: " << minHeap.top() << " (expected: 1)" << endl;
    minHeap.pop();
    cout << "After pop: " << minHeap.top() << " (expected: 3)" << endl;
    cout << endl;
}

void testSingleElement() {
    cout << "=== Test 6: Single Element ===" << endl;
    MinHeap<int> minHeap(10);
    
    minHeap.push(42);
    cout << "Inserted single element: 42" << endl;
    cout << "Top: " << minHeap.top() << " (expected: 42)" << endl;
    cout << "Size: " << minHeap.size() << " (expected: 1)" << endl;
    
    minHeap.pop();
    cout << "After pop - Size: " << minHeap.size() << " (expected: 0)" << endl;
    cout << endl;
}

void testFloatHeap() {
    cout << "=== Test 7: Heap with Floats ===" << endl;
    MinHeap<float> minHeap(10);
    
    minHeap.push(3.14f);
    minHeap.push(2.71f);
    minHeap.push(1.41f);
    minHeap.push(9.99f);
    
    cout << "Inserted: 3.14, 2.71, 1.41, 9.99" << endl;
    cout << "Min: " << minHeap.top() << " (expected: 1.41)" << endl;
    minHeap.pop();
    cout << "After pop: " << minHeap.top() << " (expected: 2.71)" << endl;
    cout << endl;
}

int main() {
    cout << "======================================" << endl;
    cout << "    Heap Test Suite" << endl;
    cout << "======================================" << endl;
    cout << endl;
    
    testMinHeapBasic();
    testMaxHeapBasic();
    testMinHeapOrdering();
    testMaxHeapOrdering();
    testHeapWithDuplicates();
    testSingleElement();
    testFloatHeap();
    
    cout << "======================================" << endl;
    cout << "    All Tests Complete" << endl;
    cout << "======================================" << endl;
    
    return 0;
}
