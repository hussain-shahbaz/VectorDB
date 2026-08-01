#include <iostream>
#include <cmath>
#include "../dataStructures/AVL.h"

using namespace std;

// In-order traversal collected into an array, to check BST ordering.
void collectInOrder(AVLNode<int>* node, int* out, int& idx) {
    if (!node) return;
    collectInOrder(node->left, out, idx);
    out[idx++] = node->value;
    collectInOrder(node->right, out, idx);
}

void testBasicInsertAndSearch() {
    cout << "=== Test 1: Basic Insert and Search ===" << endl;
    AVL<int> tree;

    tree.insert(50);
    tree.insert(30);
    tree.insert(70);
    tree.insert(20);
    tree.insert(40);

    cout << "Inserted: 50, 30, 70, 20, 40" << endl;
    cout << "Contains 40: " << (tree.contains(40) ? "YES" : "NO") << " (expected: YES)" << endl;
    cout << "Contains 99: " << (tree.contains(99) ? "YES" : "NO") << " (expected: NO)" << endl;

    cout << endl;
}

void testInOrderIsSorted() {
    cout << "=== Test 2: In-Order Traversal is Sorted ===" << endl;
    AVL<int> tree;
    int values[] = {50, 30, 70, 20, 40, 60, 80, 10, 25};
    int n = 9;

    for (int i = 0; i < n; i++) tree.insert(values[i]);

    int out[9];
    int idx = 0;
    collectInOrder(tree.getRoot(), out, idx);

    bool sorted = true;
    for (int i = 1; i < idx; i++) {
        if (out[i - 1] > out[i]) sorted = false;
    }

    cout << "In-order: ";
    for (int i = 0; i < idx; i++) cout << out[i] << " ";
    cout << endl;
    cout << (sorted ? "PASS" : "FAIL") << ": in-order traversal is sorted" << endl;

    cout << endl;
}

void testStaysBalancedOnSortedInsertion() {
    cout << "=== Test 3: Stays Balanced on Sorted Insertion ===" << endl;
    AVL<int> tree;
    int n = 1000;

    for (int i = 0; i < n; i++) tree.insert(i);

    int h = tree.height();
    // A plain BST fed sorted input degenerates to a chain (height ~= n).
    // A correct AVL tree keeps height within ~1.44*log2(n) of balanced.
    int bound = (int)(2.0 * (log(n + 1) / log(2))) + 2;

    cout << "Inserted 0.." << (n - 1) << " in sorted order" << endl;
    cout << "Height: " << h << " (expected: well under " << n << ", bound ~" << bound << ")" << endl;
    cout << (h <= bound ? "PASS" : "FAIL") << ": tree stayed balanced instead of degenerating to a chain" << endl;

    cout << endl;
}

void testRemoveLeaf() {
    cout << "=== Test 4: Remove Leaf Node ===" << endl;
    AVL<int> tree;
    tree.insert(50);
    tree.insert(30);
    tree.insert(70);

    tree.remove(30);
    cout << "Removed leaf 30" << endl;
    cout << "Contains 30: " << (tree.contains(30) ? "YES (ERROR)" : "NO (correct)") << endl;
    cout << "Contains 50: " << (tree.contains(50) ? "YES" : "NO (ERROR)") << " (expected: YES)" << endl;
    cout << "Contains 70: " << (tree.contains(70) ? "YES" : "NO (ERROR)") << " (expected: YES)" << endl;

    cout << endl;
}

void testRemoveNodeWithTwoChildren() {
    cout << "=== Test 5: Remove Node with Two Children ===" << endl;
    AVL<int> tree;
    int values[] = {50, 30, 70, 20, 40, 60, 80};
    for (int i = 0; i < 7; i++) tree.insert(values[i]);

    tree.remove(30); // has children 20 and 40
    cout << "Removed 30 (had two children: 20, 40)" << endl;
    cout << "Contains 30: " << (tree.contains(30) ? "YES (ERROR)" : "NO (correct)") << endl;
    cout << "Contains 20: " << (tree.contains(20) ? "YES" : "NO (ERROR)") << " (expected: YES)" << endl;
    cout << "Contains 40: " << (tree.contains(40) ? "YES" : "NO (ERROR)") << " (expected: YES)" << endl;

    int out[6];
    int idx = 0;
    collectInOrder(tree.getRoot(), out, idx);
    bool sorted = true;
    for (int i = 1; i < idx; i++) if (out[i - 1] > out[i]) sorted = false;
    cout << (sorted ? "PASS" : "FAIL") << ": tree still sorted after removal" << endl;

    cout << endl;
}

void testRemoveRoot() {
    cout << "=== Test 6: Remove Root Repeatedly ===" << endl;
    AVL<int> tree;
    for (int i = 1; i <= 10; i++) tree.insert(i);

    for (int i = 0; i < 5; i++) {
        int root = tree.getRoot()->value;
        tree.remove(root);
    }

    cout << "Removed the root 5 times in a row" << endl;
    int out[5];
    int idx = 0;
    collectInOrder(tree.getRoot(), out, idx);
    cout << "Remaining count: " << idx << " (expected: 5)" << endl;
    bool sorted = true;
    for (int i = 1; i < idx; i++) if (out[i - 1] > out[i]) sorted = false;
    cout << (sorted ? "PASS" : "FAIL") << ": tree still valid after repeated root removal" << endl;

    cout << endl;
}

void testEmptyTree() {
    cout << "=== Test 7: Empty Tree ===" << endl;
    AVL<int> tree;

    cout << "isEmpty: " << (tree.isEmpty() ? "YES" : "NO") << " (expected: YES)" << endl;
    cout << "Contains 5 on empty tree: " << (tree.contains(5) ? "YES (ERROR)" : "NO (correct)") << endl;
    cout << "Height of empty tree: " << tree.height() << " (expected: -1)" << endl;

    tree.remove(5); // should not crash
    cout << "Remove on empty tree: no crash (correct)" << endl;

    cout << endl;
}

void testDuplicateInsert() {
    cout << "=== Test 8: Duplicate Insert ===" << endl;
    AVL<int> tree;
    tree.insert(10);
    tree.insert(10);
    tree.insert(10);

    cout << "Inserted 10 three times" << endl;
    cout << "Contains 10: " << (tree.contains(10) ? "YES" : "NO (ERROR)") << " (expected: YES)" << endl;
    cout << "No crash on duplicate insert (correct)" << endl;

    cout << endl;
}

int main() {
    cout << "======================================" << endl;
    cout << "    AVL Tree Test Suite" << endl;
    cout << "======================================" << endl;
    cout << endl;

    testBasicInsertAndSearch();
    testInOrderIsSorted();
    testStaysBalancedOnSortedInsertion();
    testRemoveLeaf();
    testRemoveNodeWithTwoChildren();
    testRemoveRoot();
    testEmptyTree();
    testDuplicateInsert();

    cout << "======================================" << endl;
    cout << "    All Tests Complete" << endl;
    cout << "======================================" << endl;

    return 0;
}
