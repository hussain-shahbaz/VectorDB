#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include "Vector.h"
using namespace std;

template<typename T>
class BTreeNode {
public:
    Vector<T> keys;               // keys in node
    Vector<BTreeNode<T>*> children; // child pointers
    bool leaf;

    BTreeNode(bool isLeaf) : leaf(isLeaf) {}

    void printNode(int level = 0) {
        for (int i = 0; i < level; i++) cout << "  ";
        cout << "[";
        for (int i = 0; i < keys.length(); i++) {
            cout << keys[i];
            if (i < keys.length() - 1) cout << ", ";
        }
        cout << "]\n";
        if (!leaf) {
            for (int i = 0; i < children.length(); i++) {
                if (children[i])
                    children[i]->printNode(level + 1);
            }
        }
    }
};

template<typename T>
class BTree {
private:
    BTreeNode<T>* root;
    int t; // minimum degree

    void splitChild(BTreeNode<T>* parent, int index, BTreeNode<T>* child) {
        BTreeNode<T>* newNode = new BTreeNode<T>(child->leaf);
        int mid = t - 1;

        // move keys
        for (int i = mid + 1; i < child->keys.length(); i++)
            newNode->keys.push_back(child->keys[i]);
        // resize original child
        child->keys.resize(mid);

        // move children if not leaf
        if (!child->leaf) {
            for (int i = mid + 1; i < child->children.length(); i++)
                newNode->children.push_back(child->children[i]);
            child->children.resize(mid + 1);
        }

        parent->children.push_back(nullptr); // extend parent
        for (int i = parent->children.length() - 1; i > index + 1; i--)
            parent->children[i] = parent->children[i - 1];
        parent->children[index + 1] = newNode;

        parent->keys.push_back(T()); // extend key
        for (int i = parent->keys.length() - 1; i > index; i--)
            parent->keys[i] = parent->keys[i - 1];
        parent->keys[index] = child->keys[mid];
    }

    void insertNonFull(BTreeNode<T>* node, const T& key) {
        int i = node->keys.length() - 1;
        if (node->leaf) {
            node->keys.push_back(T());
            while (i >= 0 && key < node->keys[i]) {
                node->keys[i + 1] = node->keys[i];
                i--;
            }
            node->keys[i + 1] = key;
        } else {
            while (i >= 0 && key < node->keys[i])
                i--;
            i++;
            if (node->children[i]->keys.length() == 2 * t - 1) {
                splitChild(node, i, node->children[i]);
                if (key > node->keys[i])
                    i++;
            }
            insertNonFull(node->children[i], key);
        }
    }


    T* searchNode(BTreeNode<T>* node, const T& key) {
        if (!node) return nullptr;

        int i = 0;

        while (i < node->keys.length() && key > node->keys[i])
            i++;


        if (i < node->keys.length() && key == node->keys[i])
            return &node->keys[i];


        if (node->leaf)
            return nullptr;


        return searchNode(node->children[i], key);
    }

public:
    BTree(int _t) : t(_t), root(nullptr) {}

    ~BTree() {
        destroyTree(root);
    }

    void insert(const T& key) {
        if (!root) {
            root = new BTreeNode<T>(true);
            root->keys.push_back(key);
        } else {
            if (root->keys.length() == 2 * t - 1) {
                BTreeNode<T>* newRoot = new BTreeNode<T>(false);
                newRoot->children.push_back(root);
                splitChild(newRoot, 0, root);
                root = newRoot;
            }
            insertNonFull(root, key);
        }
    }

    // Search for a key and return pointer to it (nullptr if not found)
    T* search(const T& key) {
        return searchNode(root, key);
    }

    // Check if key exists
    bool contains(const T& key) {
        return search(key) != nullptr;
    }

    void print() {
        if (root) root->printNode();
    }

private:
    // Memory cleanup
    void destroyTree(BTreeNode<T>* node) {
        if (!node) return;
        if (!node->leaf) {
            for (int i = 0; i < node->children.length(); i++)
                destroyTree(node->children[i]);
        }
        delete node;
    }
};

#endif
