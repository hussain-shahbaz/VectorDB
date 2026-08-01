#ifndef HASHMAP_H
#define HASHMAP_H

#include <iostream>
#include "Vector.h"
using namespace std;

// STATE ENUM
enum State {EMPTY,OCCUPIED,DELETED};

//HashMap key value pair
template <typename K,typename V>
class Entry{
public:
    K key;
    V value;
    State state;
    Entry() : state(EMPTY){}
    Entry(const K& k,const V& val) : key(k), value(val), state(OCCUPIED) {}
};

// Hashmap
template <typename K,typename V>
class HashMap{
public:
    int size;
    int capacity;
    Vector<Entry<K,V>> table;
    double threshold;

private:

    double loadFactor() const{
        return (double) size / capacity;
    }

    int hashCode(const int& key) const {
        return key*31;
    }

    int hash1(const int& key){
        return hashCode(key) % capacity;
    }

    int hash2(const int& key){
        int h = hashCode(key);
        return 1 + (h%(capacity -1));
    }
    
    int probe(const K& key, int attempt){
        return (hash1(key) + attempt*hash2(key)) %capacity;
    }

    void rehash(){
        int oldCap = capacity;
        capacity = capacity * 2 + 1;

        Vector<Entry<K,V>> oldTab = table;
        table = Vector<Entry<K,V>>(capacity);
        size = 0;

        for(int i=0;i<oldCap;i++){
            if(oldTab[i].state == OCCUPIED){
                insert(oldTab[i].key,oldTab[i].value);
            }
        }
    }

public:
    HashMap(int cap = 7){
        capacity = cap;
        size = 0;
        threshold = 0.7;
        table = Vector<Entry<K,V>>(capacity);
    }

    void insert(const K& key,const V& value){
        if(loadFactor() >= 0.7){
            rehash();
        }
        int attempt = 0;
        while(attempt < capacity){
            int index = probe(key,attempt);

            if(table[index].state == EMPTY || table[index].state == DELETED){
                table[index] = Entry<K,V>(key,value);
                size ++;
                return;
            }

            else if(table[index].state == OCCUPIED && table[index].key == key){
                table[index].value  = value;
                return;
            }

            attempt ++;
        }
    }
    bool contains(const K& key){
    int attempt = 0;
    while(attempt < capacity){
        int index = probe(key, attempt);

        if(table[index].state == EMPTY)
            return false;

        if(table[index].state == OCCUPIED && table[index].key == key)
            return true;

        attempt++;
    }
    return false;
}

    V* find(const K& key){
        int attempt = 0;

        while(attempt < capacity){
            int index = probe(key,attempt);
            
            if(table[index].state == EMPTY){
                return nullptr;
            } 
            if(table[index].state == OCCUPIED && table[index].key == key){
                return &table[index].value;
            } 

            attempt ++;
        }
        return nullptr;
    }
    bool remove(const K& key){
        int attempt = 0;
        while(attempt < capacity){
            int index = probe(key,attempt);

            if(table[index].state == EMPTY) return false;

            if(table[index].state == OCCUPIED && table[index].key == key){
                table[index].state = DELETED;
                size --;
                return true;
            }

            attempt ++;
        }
        return false;
    }

    
};

#endif
