#ifndef VECTOR_H
#define VECTOR_H

#include<iostream>
using namespace std;

template <typename T>
class Vector{
    T* arr;
    int size;
public:
    int capacity;
    public:
    Vector(){
        arr = new T[1];
        size = 0;
        capacity = 1;
    }
    Vector(int s){
        arr = new T[s];
        size = s;
        capacity = s * 2;
    }
    Vector(const Vector<T>& other){
        if(this != &other){
            size = other.size;
            capacity = other.capacity;
            arr = new T[capacity];
            for(int i = 0; i < size; i++)
            arr[i] = other.arr[i];
        }
    }
    ~Vector(){
        delete[] arr;
    }
    
    void swap(int i,int j){
        T temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
    
    Vector<T>& operator=(const Vector<T>& other){
        if(this != &other){
            delete[] arr;
            size = other.size;
            capacity = other.capacity;
            arr = new T[capacity];
            for(int i = 0; i < size; i++)
                arr[i] = other.arr[i];
        }
        return *this;
    }
    const T& operator[](int index) const {
        return arr[index];
    }
    
    T& operator[](int index){
        return arr[index];
    }
    
    int length() const {
        return size;
    }

    T& top(){
        if(size>0)
            return arr[size-1];
        return arr[0]; //To be adjusted
    }

    void print(){
        cout<<"[";
        for(int i=0;i<size;i++){
            cout<<arr[i];
            if(i != size-1){
                cout<<",";
            }
        }
        cout<<"]"<<endl;
    }
    
    
    bool empty() const {
        return size == 0;
    }
    
    void push_back(const T& val){
        if(size == capacity){
            T* newArr = new T[capacity * 2];
            for(int i = 0; i < size; i++)
                newArr[i] = arr[i];
            delete[] arr;
            arr = newArr;
            capacity *= 2;
        }
        arr[size++] = val;
    }
    
    void pop_back(){
        if(size > 0) size--;
    }
    
    void clear(){
        size = 0;
    }
    
    void reserve(int newcapacity){
        if(newcapacity > capacity){
            T* newArr = new T[newcapacity];
            for(int i = 0; i < size; i++)
                newArr[i] = arr[i];
            delete[] arr;
            arr = newArr;
            capacity = newcapacity;
        }
    }
    
    void resize(int newSize){
        if(newSize > capacity)
            reserve(newSize * 2);
        size = newSize;
    }
    
    T* data(){
        return arr;
    }
    
    const T* data() const {
        return arr;
    }


};

#endif
