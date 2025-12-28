#ifndef STACK_H
#define STACK_H

#include<iostream>
#include "List.h"


template <typename T>
class IStack{
    public:
    virtual void push(const T& value)  = 0;
    virtual void pop()  = 0;
    virtual T& top() = 0;
    virtual bool isEmpty() = 0;
    virtual bool isFull() = 0;
    virtual void print() = 0;
    ~IStack() {}
};


template <typename T>
class Stack : public IStack<T>{
    T* arr;
    int capacity;
    int idx;
public:

    Stack(int cap = 20) : capacity(cap), idx(-1) {
        arr = new T[capacity];
    }

    Stack(const Stack<T>& other) {
        capacity = other.capacity;
        idx = other.idx;

        arr = new T[capacity];
        for (int i = 0; i <= idx; i++) {
            arr[i] = other.arr[i];
        }
    }
    
    T& operator[](int i){
        if(i < 0 || i > idx){
            cout<<"Stack Overflow"<<endl;
            exit(1);
        }
        return arr[i];
    }
    
    int size() const {
        return idx + 1;
    }

    bool isEmpty() override {return idx == -1;}

    void push(const T& val) override {
        if(idx + 1 == capacity){
            cout<<"Stack overflow"<<endl;
            exit(1);
        }
        arr[++idx] = val;
    }

    void pop() override {
        if(isEmpty()){
            cout<<"Stack Underflow"<<endl;
            return;
        }
        idx--;
    }

    T& top() override {
        if(!isEmpty()) return arr[idx];
        cout << "Stack empty" << endl;
        exit(1);
    }

    bool isFull() override { return idx == capacity -1;}
    
    void print()  override {
        for(int i=0;i<=idx;i++){
            cout<<arr[i]<<" ";
        }
        cout<<endl;
    }

    ~Stack(){
        delete[] arr;
    }
};
 
template <typename T>
class ListStack : public IStack<T>{
    Node<T>* head;
public:
    ListStack() { head = NULL;}

    bool isEmpty() override {return head == NULL;}

    void push(const T& val) override {
        Node<T>* newNode = new Node<T>(val);
        newNode->next = head;
        head = newNode;
    }

    void pop() override {
        if(isEmpty()) return;
        Node<T>* temp = head;
        head = head->next;
        delete temp;
    }

    T& top() override {
        if(head) return head->data;
        cout<<"Stack empty"<<endl;
        exit(1);
    }

    bool isFull() override{ return false;}

    void print() override {
        Node<T>* temp = head;
        cout << "Top -> ";
        while (temp) {
            cout << temp->data << " ";
            temp = temp->next;
        }
        cout << "-> NULL" << endl;
    }

    ~ListStack() {
        while (!isEmpty()) pop();
    }
};

#endif