#include <iostream>
#include "../dataStructures/Graph.h"

using namespace std;

void testBasicGraph() {
    cout << "=== Test 1: Basic Graph Operations ===" << endl;
    Graph<int> graph;
    
    graph.addNode(1);
    graph.addNode(2);
    graph.addNode(3);
    graph.addNode(4);
    
    cout << "Added nodes: 1, 2, 3, 4" << endl;
    
    graph.addEdge(1, 2);
    graph.addEdge(1, 3);
    graph.addEdge(2, 4);
    graph.addEdge(3, 4);
    
    cout << "Added edges: 1-2, 1-3, 2-4, 3-4" << endl;
    
    Vector<int>* neighbors1 = graph.getNeighbors(1);
    cout << "Neighbors of 1: ";
    if(neighbors1){
        for(int i = 0; i < neighbors1->length(); i++){
            cout << (*neighbors1)[i] << " ";
        }
        cout << "(expected: 2 3 or 3 2)" << endl;
    }
    
    Vector<int>* neighbors2 = graph.getNeighbors(2);
    cout << "Neighbors of 2: ";
    if(neighbors2){
        for(int i = 0; i < neighbors2->length(); i++){
            cout << (*neighbors2)[i] << " ";
        }
        cout << "(expected: 1 4 or 4 1)" << endl;
    }
    
    cout << endl;
}

void testSingleNode() {
    cout << "=== Test 2: Single Node ===" << endl;
    Graph<int> graph;
    
    graph.addNode(1);
    cout << "Added single node: 1" << endl;
    
    Vector<int>* neighbors = graph.getNeighbors(1);
    cout << "Neighbors of 1: ";
    if(neighbors){
        cout << neighbors->length() << " (expected: 0)" << endl;
    } else {
        cout << "nullptr" << endl;
    }
    
    cout << endl;
}

void testMultipleEdges() {
    cout << "=== Test 3: Multiple Edges from Same Node ===" << endl;
    Graph<int> graph;
    
    for(int i = 0; i < 5; i++){
        graph.addNode(i);
    }
    
    graph.addEdge(0, 1);
    graph.addEdge(0, 2);
    graph.addEdge(0, 3);
    graph.addEdge(0, 4);
    
    cout << "Node 0 connected to: 1, 2, 3, 4" << endl;
    
    Vector<int>* neighbors = graph.getNeighbors(0);
    cout << "Neighbors of 0: ";
    if(neighbors){
        for(int i = 0; i < neighbors->length(); i++){
            cout << (*neighbors)[i] << " ";
        }
        cout << endl;
        cout << "Count: " << neighbors->length() << " (expected: 4)" << endl;
    }
    
    cout << endl;
}

void testDisconnectedNodes() {
    cout << "=== Test 4: Disconnected Nodes ===" << endl;
    Graph<int> graph;
    
    graph.addNode(1);
    graph.addNode(2);
    graph.addNode(3);
    
    graph.addEdge(1, 2);
    
    cout << "Nodes: 1, 2, 3" << endl;
    cout << "Edges: 1-2 only" << endl;
    
    Vector<int>* neighbors3 = graph.getNeighbors(3);
    cout << "Neighbors of 3 (disconnected): ";
    if(neighbors3){
        cout << neighbors3->length() << " (expected: 0)" << endl;
    }
    
    cout << endl;
}

void testUndirectedEdges() {
    cout << "=== Test 5: Undirected Edges ===" << endl;
    Graph<int> graph;
    
    graph.addNode(1);
    graph.addNode(2);
    
    graph.addEdge(1, 2);
    
    cout << "Added edge: 1-2" << endl;
    
    Vector<int>* neighbors1 = graph.getNeighbors(1);
    Vector<int>* neighbors2 = graph.getNeighbors(2);
    
    cout << "Neighbors of 1: ";
    if(neighbors1){
        for(int i = 0; i < neighbors1->length(); i++){
            cout << (*neighbors1)[i] << " ";
        }
        cout << endl;
    }
    
    cout << "Neighbors of 2: ";
    if(neighbors2){
        for(int i = 0; i < neighbors2->length(); i++){
            cout << (*neighbors2)[i] << " ";
        }
        cout << endl;
    }
    
    cout << "(Both should list the other)" << endl;
    cout << endl;
}

void testStringGraph() {
    cout << "=== Test 6: String Nodes ===" << endl;
    Graph<int> graph;
    
    graph.addNode(10);
    graph.addNode(20);
    graph.addNode(30);
    
    graph.addEdge(10, 20);
    graph.addEdge(20, 30);
    
    cout << "Nodes: 10, 20, 30" << endl;
    cout << "Edges: 10-20, 20-30" << endl;
    
    Vector<int>* neighbors20 = graph.getNeighbors(20);
    cout << "Neighbors of 20: ";
    if(neighbors20){
        for(int i = 0; i < neighbors20->length(); i++){
            cout << (*neighbors20)[i] << " ";
        }
        cout << endl;
        cout << "Count: " << neighbors20->length() << " (expected: 2)" << endl;
    }
    
    cout << endl;
}

void testCompleteGraph() {
    cout << "=== Test 7: Complete Graph (4 nodes) ===" << endl;
    Graph<int> graph;
    
    for(int i = 0; i < 4; i++){
        graph.addNode(i);
    }
    
    for(int i = 0; i < 4; i++){
        for(int j = i + 1; j < 4; j++){
            graph.addEdge(i, j);
        }
    }
    
    cout << "Created complete graph with 4 nodes" << endl;
    
    for(int i = 0; i < 4; i++){
        Vector<int>* neighbors = graph.getNeighbors(i);
        cout << "Node " << i << " has " << (neighbors ? neighbors->length() : 0) << " neighbors (expected: 3)" << endl;
    }
    
    cout << endl;
}

int main() {
    cout << "======================================" << endl;
    cout << "    Graph Test Suite" << endl;
    cout << "======================================" << endl;
    cout << endl;
    
    testBasicGraph();
    testSingleNode();
    testMultipleEdges();
    testDisconnectedNodes();
    testUndirectedEdges();
    testStringGraph();
    testCompleteGraph();
    
    cout << "======================================" << endl;
    cout << "    All Tests Complete" << endl;
    cout << "======================================" << endl;
    
    return 0;
}
