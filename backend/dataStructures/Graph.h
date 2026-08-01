#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include "Vector.h"
#include "HashMap.h"
using namespace std;

// Undirected graph keyed by node label (not position), backed by a HashMap
// from label -> adjacency list. addNode() with no argument auto-labels with
// the next sequential index (as T), which is what GraphANN relies on: it
// calls addNode() once per VectorDB position, so auto-labels line up with
// VectorDB indices exactly like the old position-only implementation did.
template<typename T>
class Graph {
private:
    HashMap<T, Vector<T>> adjacency;
    Vector<T> nodeOrder; // insertion order, for iteration/print

public:
    Graph() {}

    void addNode() {
        addNode((T)nodeOrder.length());
    }

    void addNode(const T& label) {
        if (adjacency.find(label) != nullptr) return;
        adjacency.insert(label, Vector<T>());
        nodeOrder.push_back(label);
    }

    void addEdge(const T& u, const T& v) {
        Vector<T>* uAdj = adjacency.find(u);
        Vector<T>* vAdj = adjacency.find(v);
        if (!uAdj || !vAdj) return;

        uAdj->push_back(v);
        vAdj->push_back(u);
    }

    bool hasNode(const T& u) {
        return adjacency.find(u) != nullptr;
    }

    // Returns nullptr if the node doesn't exist.
    Vector<T>* getNeighbors(const T& u) {
        return adjacency.find(u);
    }

    // Reference-returning variant for callers (e.g. GraphANN) that already
    // know the node exists; throws otherwise.
    Vector<T>& neighbors(const T& u) {
        Vector<T>* n = adjacency.find(u);
        if (!n) throw runtime_error("Node does not exist");
        return *n;
    }

    int size() const {
        return nodeOrder.length();
    }

    void print() {
        for (int i = 0; i < nodeOrder.length(); i++) {
            T u = nodeOrder[i];
            cout << u << ": ";
            Vector<T>& adj = *adjacency.find(u);
            for (int j = 0; j < adj.length(); j++) cout << adj[j] << " ";
            cout << endl;
        }
    }
};

#endif
