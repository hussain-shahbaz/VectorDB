#ifndef HEAP_H
#define HEAP_H

#include <iostream>
#include "Vector.h"

template<typename T>
class Heap {
protected:
    Vector<T> heap;

    virtual bool compare(const T& a, const T& b) const = 0;

    int getParentIndex(int i)    { return (i - 1) / 2; }

    int getLeftChildIndex(int i) { return 2 * i + 1; }

    int getRightChildIndex(int i){ return 2 * i + 2; }

    bool hasParent(int i)     { return i > 0; }

    bool hasLeftChild(int i)  { return getLeftChildIndex(i) < heap.length(); }

    bool hasRightChild(int i) { return getRightChildIndex(i) < heap.length(); }

    void heapifyUp() {
        int index = heap.length() - 1;
        while (hasParent(index) && compare(heap[index], heap[getParentIndex(index)])) {
            heap.swap(index, getParentIndex(index));
            index = getParentIndex(index);
        }
    }

    void heapifyDown() {
        int index = 0;
        while (hasLeftChild(index)) {
            int targetIndex = getLeftChildIndex(index);
            if (hasRightChild(index) && compare(heap[getRightChildIndex(index)], heap[targetIndex])) {
                targetIndex = getRightChildIndex(index);
            }

            if (compare(heap[targetIndex], heap[index])) {
                heap.swap(index, targetIndex);
                index = targetIndex;
            } else break;
        }
    }

public:
    Heap(int size) {
    }

    virtual ~Heap() {
    }

    void push(const T& value) {
        heap.push_back(value);
        heapifyUp();
    }

    void pop() {
        if (heap.length() == 0) return;
        heap[0] = heap[heap.length() - 1];
        heap.pop_back();
        if (heap.length() > 0) heapifyDown();
    }

    T top() const {
        if (heap.length() == 0) throw std::runtime_error("Heap is empty");
        return heap[0];
    }

    int size() const {
        return heap.length();
    }
};

template<typename T>
class MaxHeap : public Heap<T> {
protected:
    bool compare(const T& a, const T& b) const override {
        return a > b;
    }

public:
    MaxHeap(int size) : Heap<T>(size) {}
};

template<typename T>
class MinHeap : public Heap<T> {
protected:
    bool compare(const T& a, const T& b) const override {
        return a < b;
    }

public:
    MinHeap(int size) : Heap<T>(size) {}
};

#endif