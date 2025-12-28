#ifndef AVL_H
#define AVL_H

#include <iostream>
using namespace std;

int SPACE = 5;

template<typename T>
class AVLNode {
public:
    T value;
    int height;
    AVLNode* left;
    AVLNode* right;
    AVLNode* parent;

    AVLNode(const T& val){
        value = val;
        height = 0;
        left = right = parent = nullptr;
    }
};

template<typename T>
class AVL {
private:
    AVLNode<T>* root;

    int height(AVLNode<T>* node){
        return node ? node->height : -1;
    }

    void updateHeight(AVLNode<T>* node){
        int leftHeight = height(node->left);
        int rightHeight = height(node->right);
        node->height = 1 + max(leftHeight, rightHeight);
    }

    int balanceFactor(AVLNode<T>* node){
        return height(node->left) - height(node->right);
    }

    AVLNode<T>* leftRotate(AVLNode<T>* x){
        AVLNode<T>* y = x->right;
        AVLNode<T>* newSubtree = y->left;

        y->left = x;
        x->right = newSubtree;

        if(newSubtree) newSubtree->parent = x;

        y->parent = x->parent;
        if(!x->parent) root = y;
        else if(x == x->parent->left) x->parent->left = y;
        else x->parent->right = y;

        x->parent = y;

        updateHeight(x);
        updateHeight(y);
        return y;
    }

    AVLNode<T>* rightRotate(AVLNode<T>* y){
        AVLNode<T>* x = y->left;
        AVLNode<T>* newSubtree = x->right;

        x->right = y;
        y->left = newSubtree;

        if(newSubtree) newSubtree->parent = y;

        x->parent = y->parent;
        if(!y->parent) root = x;
        else if(y == y->parent->left) y->parent->left = x;
        else y->parent->right = x;

        y->parent = x;

        updateHeight(y);
        updateHeight(x);
        return x;
    }

    AVLNode<T>* rebalance(AVLNode<T>* node){
        int bf = balanceFactor(node);

        if(bf > 1){
            if(balanceFactor(node->left) >= 0) return rightRotate(node);
            else {
                node->left = leftRotate(node->left);
                node->left->parent = node;
                return rightRotate(node);
            }
        }
        if(bf < -1){
            if(balanceFactor(node->right) <= 0) return leftRotate(node);
            else {
                node->right = rightRotate(node->right);
                node->right->parent = node;
                return leftRotate(node);
            }
        }
        return node;
    }

    AVLNode<T>* insert(AVLNode<T>* node, AVLNode<T>* parent, const T& value){
        if(!node){
            AVLNode<T>* newNode = new AVLNode<T>(value);
            newNode->parent = parent;
            return newNode;
        }

        if(value < node->value) node->left = insert(node->left, node, value);
        else if(value > node->value) node->right = insert(node->right, node, value);

        updateHeight(node);
        AVLNode<T>* newRoot = rebalance(node);
        if(!newRoot->parent) root = newRoot;
        return newRoot;
    }

    AVLNode<T>* findMin(AVLNode<T>* node){
        while(node->left) node = node->left;
        return node;
    }

    AVLNode<T>* deleteNode(AVLNode<T>* node, const T& value){
        if(!node) return nullptr;

        if(value < node->value){
            node->left = deleteNode(node->left, value);
            if(node->left) node->left->parent = node;
        }
        else if(value > node->value){
            node->right = deleteNode(node->right, value);
            if(node->right) node->right->parent = node;
        }
        else {
            if(!node->left || !node->right){
                AVLNode<T>* temp = node->left ? node->left : node->right;
                if(!temp){
                    temp = node;
                    node = nullptr;
                } else {
                    temp->parent = node->parent;
                    *node = *temp;
                }
                delete temp;
            } else {
                AVLNode<T>* temp = findMin(node->right);
                node->value = temp->value;
                node->right = deleteNode(node->right, temp->value);
                if(node->right) node->right->parent = node;
            }
        }

        if(!node) return node;
        updateHeight(node);
        return rebalance(node);
    }

    void visualizeTree(AVLNode<T>* node, int space){
        if(!node) return;
        space += SPACE;
        visualizeTree(node->right, space);

        cout << endl;
        for(int i=SPACE;i<space;i++) cout<<" ";
        cout << node->value << "\n";

        visualizeTree(node->left, space);
    }

public:
    AVL() : root(nullptr) {}
    ~AVL() { clear(root); }

    void clear(AVLNode<T>* node){
        if(!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

    void insert(const T& value){ root = insert(root, nullptr, value); }
    void remove(const T& value){ root = deleteNode(root, value); }

    AVLNode<T>* getRoot() { return root; }
    void printTree() { visualizeTree(root, 0); }
};

#endif
