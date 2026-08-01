#ifndef BTREEINDEX_H
#define BTREEINDEX_H

#include "../../dataStructures/BTree.h"
#include "../../dataStructures/Pair.h"

// Maps vector id -> its position (0-indexed) within the vector array of a
// loaded/saved file. BTree<T> is a set, not a map, but Pair<K,V>'s
// comparison operators only look at .key — so a BTree<Pair<int,int>> keyed
// by id works as an id -> position index for free, searchable with a
// placeholder Pair(id, don't-care).
class BtreeIndex {
private:
    BTree<Pair<int, int>> tree;

public:
    BtreeIndex(int degree = 3) : tree(degree) {}

    void insert(int id, int position) {
        tree.insert(Pair<int, int>(id, position));
    }

    bool find(int id, int& position) {
        Pair<int, int> key(id, -1);
        Pair<int, int>* found = tree.search(key);
        if (!found) return false;
        position = found->value;
        return true;
    }

    bool contains(int id) {
        Pair<int, int> key(id, -1);
        return tree.contains(key);
    }

    void clear() {
        tree.clear();
    }
};

#endif
