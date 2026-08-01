#ifndef LIST_H
#define LIST_H

#include <iostream>
using namespace std;

template<typename T>
class Node {
public:
    T data;
    Node<T>* next;

    Node(const T& d) {
        data = d;
        next = nullptr;
    }
};

template<typename T>
class List {
private:
    Node<T>* head;
    Node<T>* tail;
    int count;
public:
    List() { head = nullptr; tail = nullptr; count = 0; }

    List(const List<T>& other) {
        head = tail = nullptr;
        count = 0;
        Node<T>* temp = other.head;
        while (temp) {
            insertAtEnd(temp->data);
            temp = temp->next;
        }
    }
    ~List() {
        Node<T>* temp;
        while (head) {
            temp = head;
            head = head->next;
            delete temp;
        }
    }

    Node<T>* getHead(){
        return head;
    }

    int size() const {
        return count;
    }

    T& top(){
        if(isEmpty()){
            cout<<"List is empty"<<endl;
            exit(1);
        }
        Node<T>* temp = head;
        while(temp->next) temp = temp->next;
        return temp->data;
    }

    bool isEmpty() const { return head == nullptr; }



    Node<T>* insertAtEnd(const T& x) {
        Node<T>* newNode = new Node<T>(x);
        if(isEmpty()){
            head = tail = newNode;
        } else {
            tail->next = newNode;
            tail = newNode;
        }
        count++;
        return head;
    }

    Node<T>* insertAtFront(const T& x) {
        Node<T>* newNode = new Node<T>(x);
        if(isEmpty()){
            head = tail = newNode;
        } else {
            newNode->next = head;
            head = newNode;
        }
        count++;
        return head;
    }

    Node<T>* removeList(const T& x){
        if(isEmpty()){
            return nullptr;
        }

        if(head->data == x){
            Node<T>* removed = head;
            head = tail = nullptr;
            count = 0;
            return removed;
        }

        Node<T>* temp = head;
        while(temp && temp->next){
            if(temp->next->data == x){
                break;
            }
            temp = temp->next;
        }

        if(temp == nullptr || temp->next == nullptr){
            return nullptr;
        }

        Node<T>* removed = temp->next;
        temp->next = nullptr;
        tail = temp;
        count--;
        return removed;
    }
    bool contains(const T& card){
        Node<T>* temp = head;
        while(temp != NULL){
            if(temp->data == card) return true;
            temp = temp->next;
        }
        return false;
    }

    bool search(const T& x) {
        return contains(x);
    }

    bool remove(const T& x) {
        if (head == nullptr)
            return false;

        if (head->data == x) {
            Node<T>* del = head;
            head = head->next;
            if (head == nullptr) tail = nullptr;
            delete del;
            count--;
            return true;
        }

        Node<T>* temp = head;
        while (temp->next != nullptr && !(temp->next->data == x)) {
            temp = temp->next;
        }

        if (temp->next == nullptr)
            return false;

        Node<T>* del = temp->next;
        temp->next = del->next;
        if (del == tail) tail = temp;
        delete del;
        count--;
        return true;
    }

    Node<T>* insertList(Node<T>* other){
        int added = 0;
        for(Node<T>* n = other; n != nullptr; n = n->next) added++;

        if(isEmpty()){
            head = other;
            Node<T>* temp = other;
            while(temp->next){
                temp = temp->next;
            }
            tail = temp;
            count = added;
            return head;
        }
        tail->next = other;
        Node<T>* temp = other;
        while(temp->next){
            temp = temp->next;
        }
        tail = temp;
        count += added;
        return head;
    }


    void clear(){
        Node<T>* temp;
        while (head) {
            temp = head;
            head = head->next;
            delete temp;
        }
        tail = nullptr;
        count = 0;
    }

    void print() const {
        Node<T>* temp = head;
        while (temp) {
            cout << temp->data << " -> ";
            temp = temp->next;
        }
        cout << "NULL" << endl;
    }

};

#endif
