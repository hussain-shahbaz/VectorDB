#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include "Vector.h"
#include"HashMap.h"
using namespace std;

template<typename T>
class Graph {
private:
    Vector< Vector<T> > adj;  

public:
    Graph() {}


    void addNode() {
        adj.push_back(Vector<T>());
    }

    void addEdge(int u, int v) {
        if(u >= adj.length() || v >= adj.length()) return;

        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    void addWeightedEdge(int u, int v, const T& weight) {
        if(u >= adj.length() || v >= adj.length()) return;

        adj[u].push_back(weight); // could store pair<int, T> if weighted
        adj[v].push_back(weight);
    }

    Vector<T>& neighbors(int u){
        if(u >= adj.length()) throw runtime_error("Node does not exist");
        return adj[u];
    }

    int size() const {
        return adj.length();
    }

    void print() {
        for(int i=0;i<adj.length();i++){
            cout << i << ": ";
            for(int j=0;j<adj[i].length();j++){
                cout << adj[i][j] << " ";
            }
            cout << endl;
        }
    }
};

#endif
