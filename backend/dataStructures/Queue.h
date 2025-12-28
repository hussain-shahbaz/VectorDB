#ifndef QUEUE_H
#define QUEUE_H
#include<iostream>
#include"List.h"

using namespace std;

template <typename T>
class IQueue{
    public:
    virtual void enqueue(const T& value) = 0;
    virtual void dequeue() = 0;
    virtual T& peek() = 0;
    virtual bool isEmpty() = 0;
    virtual bool isFull() = 0;
    virtual void print() = 0;
    virtual ~IQueue() {}
};

template <typename T>
class Queue : public IQueue<T>{
    T* arr;
    int capacity;
    int front,rear;
public:
    Queue(int cap=30) : capacity(cap),front(0),rear(-1){
        arr = new T[capacity];
    }

    Queue(const Queue<T>& other) {
        capacity = other.capacity;
        front = other.front;
        rear = other.rear;

        arr = new T[capacity];

        for (int i = front; i <= rear; i++) {
            arr[i] = other.arr[i];
        }
    }
    void clear(){
        delete[] arr;
        arr = new T[capacity];
        front = 0;
        rear = -1;
    }
    bool isEmpty() override {return front > rear;}

    bool isFull() override { return rear == capacity -1;}
    
    void print()  override {
        for(int i=front;i<=rear;i++){
            cout<<arr[i]<<" ";
        }
        cout<<endl;
    }
    
    void enqueue(const T& value) override {
        if(isFull()){
            cout<<"Queue Full"<<endl;
            exit(1);
        }
        arr[++rear] = value;
    }

    void dequeue() override{
        if(isEmpty()){
            cout<<"Queue Empty"<<endl;
            exit(1);
        }
        front++;
    }

    T& peek() override {
        if(isEmpty()){
            cout<<"Queue Empty"<<endl;
            exit(1);
        }
        return arr[front];
    }
    
    ~Queue() {
        delete[] arr;
    }
};


template <typename T>
class ListQueue:public IQueue<T>{
    Node<T>* head;
    Node<T>* tail;
public:
    ListQueue(){
        head = tail = NULL;
    }
    bool isEmpty() override {return head == NULL;}
    bool isFull() override {return false;}
    void enqueue(const T& value) override {
        Node<T>* newNode = new Node<T>(value);
        if(isEmpty()){
            head = tail = newNode;
            return;
        }
        tail->next = newNode;
        tail = newNode;
    }
    void dequeue() override {
        if(isEmpty()){
            cout<<"Queue Empty"<<endl;
            exit(1);
        }
        Node<T>* temp = head;
        head = head->next;
        delete temp;
        if(head == NULL) tail = NULL;
    }

    T& peek() override{
        if(isEmpty()){
            cout<<"Queue Empty"<<endl;
            exit(1);
        }
        return head->data;
    }
    void print() override {
        Node<T>* temp = head;
        cout << "Front -> ";
        while (temp) {
            cout << temp->data << " ";
            temp = temp->next;
        }
        cout << "-> Rear" << endl;
    }
    ~ListQueue() {
        while (!isEmpty()) dequeue();
    }
};
#endif