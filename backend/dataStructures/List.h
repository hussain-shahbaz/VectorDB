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
public:
    List() { head = nullptr; tail = nullptr; }

    List(const List<T>& other) {
        head = tail = nullptr;
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
        return head;
    }

    Node<T>* removeList(const T& x){  
        if(isEmpty()){
            return nullptr;
        }
        
        if(head->data == x){
            Node<T>* removed = head;
            head = tail = nullptr;
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
    bool remove(const T& x) {
        if (head == NULL)
            return false;

        if (head->data == x) {
            Node<T>* del = head;
            head = head->next;
            delete del;
            return true;
        }

        Node<T>* temp = head;
        while (temp->next != NULL && !(temp->next->data == x)) {
            temp = temp->next;
        }


        if (temp->next == NULL)
            return false;

        Node<T>* del = temp->next;
        temp->next = del->next;
        delete del;
        return head;
    }

    Node<T>* insertList(Node<T>* other){
        if(isEmpty()){
            head = other;
            Node<T>* temp = other;
            while(temp->next){
                temp = temp->next;
            }
            tail = temp;
            return head;
        }
        tail->next = other;
        Node<T>* temp = other;
        while(temp->next){
            temp = temp->next;
        }
        tail = temp;
        return head;
    }


    void clear(){
        head = tail = NULL;
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
