#include <iostream>
#include "../dataStructures/Vector.h"

using namespace std;

void testBasicOperations() {
    cout << "=== Test 1: Basic Push and Access ===" << endl;
    Vector<int> vec;
    
    vec.push_back(10);
    vec.push_back(20);
    vec.push_back(30);
    
    cout << "Pushed: 10, 20, 30" << endl;
    cout << "Size: " << vec.length() << " (expected: 3)" << endl;
    cout << "Capacity: " << vec.capacity << endl;
    
    cout << "Elements: ";
    for(int i = 0; i < vec.length(); i++){
        cout << vec[i] << " ";
    }
    cout << endl;
    
    cout << endl;
}

void testResize() {
    cout << "=== Test 2: Automatic Resize ===" << endl;
    Vector<int> vec;
    
    cout << "Initial capacity: " << vec.capacity << endl;
    
    for(int i = 0; i < 20; i++){
        vec.push_back(i);
    }
    
    cout << "After 20 pushes:" << endl;
    cout << "  Size: " << vec.length() << endl;
    cout << "  Capacity: " << vec.capacity << " (should have grown)" << endl;
    
    bool correct = true;
    for(int i = 0; i < 20; i++){
        if(vec[i] != i){
            correct = false;
            break;
        }
    }
    cout << "  Values correct: " << (correct ? "YES" : "NO") << endl;
    
    cout << endl;
}

void testPopBack() {
    cout << "=== Test 3: Pop Back ===" << endl;
    Vector<int> vec;
    
    for(int i = 0; i < 5; i++){
        vec.push_back(i * 10);
    }
    
    cout << "Initial size: " << vec.length() << endl;
    
    int val = vec[vec.length()-1];
    vec.pop_back();
    cout << "Popped value: " << val << " (expected: 40)" << endl;
    cout << "New size: " << vec.length() << " (expected: 4)" << endl;
    
    cout << "Remaining: ";
    for(int i = 0; i < vec.length(); i++){
        cout << vec[i] << " ";
    }
    cout << endl;
    
    cout << endl;
}

void testInsert() {
    cout << "=== Test 4: Insert at Position ===" << endl;

}

void testRemove() {
    cout << "=== Test 5: Remove from Position ===" << endl;
   
}

void testClear() {
    cout << "=== Test 6: Clear ===" << endl;
    Vector<int> vec;
    
    for(int i = 0; i < 10; i++){
        vec.push_back(i);
    }
    
    cout << "Before clear - Size: " << vec.length() << endl;
    
    vec.clear();
    
    cout << "After clear - Size: " << vec.length() << " (expected: 0)" << endl;
    cout << "Capacity: " << vec.capacity << endl;
    
    vec.push_back(999);
    cout << "After push_back(999) - Size: " << vec.length() << endl;
    cout << "Value: " << vec[0] << " (expected: 999)" << endl;
    
    cout << endl;
}

void testFloatVector() {
    cout << "=== Test 7: Float Vector ===" << endl;
    Vector<float> vec;
    
    vec.push_back(1.5f);
    vec.push_back(2.7f);
    vec.push_back(3.14f);
    
    cout << "Float vector: ";
    for(int i = 0; i < vec.length(); i++){
        cout << vec[i] << " ";
    }
    cout << endl;
    
    cout << endl;
}

void testCopyBehavior() {
    cout << "=== Test 8: Large Insertions ===" << endl;
    Vector<int> vec;
    
    int n = 100;
    for(int i = 0; i < n; i++){
        vec.push_back(i);
    }
    
    cout << "Inserted " << n << " elements" << endl;
    cout << "Size: " << vec.length() << endl;
    cout << "Capacity: " << vec.capacity << endl;
    
    bool correct = true;
    for(int i = 0; i < n; i++){
        if(vec[i] != i){
            correct = false;
            break;
        }
    }
    cout << "All values correct: " << (correct ? "YES" : "NO") << endl;
    
    cout << endl;
}

int main() {
    cout << "======================================" << endl;
    cout << "    Vector Test Suite" << endl;
    cout << "======================================" << endl;
    cout << endl;
    
    testBasicOperations();
    testResize();
    testPopBack();
    testInsert();
    testRemove();
    testClear();
    testFloatVector();
    testCopyBehavior();
    
    cout << "======================================" << endl;
    cout << "    All Tests Complete" << endl;
    cout << "======================================" << endl;
    
    return 0;
}
