#ifndef PAIR_H
#define PAIR_H

template <typename K, typename V>
class Pair {
public:
    K key;
    V value;
    
    Pair() : key(K()), value(V()) {}
    
    Pair(const K& k, const V& v) : key(k), value(v) {}
    
    bool operator<(const Pair& other) const {
        return key < other.key;
    }
    
    bool operator>(const Pair& other) const {
        return key > other.key;
    }
    
    bool operator==(const Pair& other) const {
        return key == other.key;
    }
    
    bool operator!=(const Pair& other) const {
        return !(*this == other);
    }
    
    bool operator<=(const Pair& other) const {
        return key <= other.key;
    }
    
    bool operator>=(const Pair& other) const {
        return key >= other.key;
    }
};

#endif