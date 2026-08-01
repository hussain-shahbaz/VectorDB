#ifndef GRAPHANN_H
#define GRAPHANN_H

#include "VectorDB.h"
#include "Similarity.h"
#include "../../dataStructures/Graph.h"
#include "../../dataStructures/Vector.h"
#include "../../dataStructures/Pair.h"
#include "../../dataStructures/Heap.h"
#include "../../dataStructures/HashMap.h"

class GraphANN {
private:
    VectorDB* db;
    Graph<int> graph;
    int maxConnections;
    
    Vector<Pair<float, int>> findKNearest(int nodeId, int k) {
        VectorItem* node = db->at(nodeId);
        MinHeap<Pair<float, int>> heap(k);

        for(int i = 0; i < db->size(); i++) {
            if(i == nodeId) continue;

            VectorItem* other = db->at(i);
            if(other->deleted) continue;

            float sim = Similarity::cosineSimilarity(node->embedding, other->embedding);
            
            if(heap.size() < k) {
                heap.push(Pair<float, int>(sim, i));
            } else if(sim > heap.top().key) {
                heap.pop();
                heap.push(Pair<float, int>(sim, i));
            }
        }
        
        Vector<Pair<float, int>> nearest;
        while(heap.size() > 0) {
            nearest.push_back(heap.top());
            heap.pop();
        }
        
        return nearest;
    }

public:
    GraphANN(VectorDB* database, int maxConn = 5) : db(database), maxConnections(maxConn) {}
    
    void buildGraph() {
        // Every position gets a node — even deleted ones — so auto-labels
        // (sequential, position-based) keep lining up with VectorDB ids.
        // Deleted items just never get outgoing edges built for them below.
        for(int i = 0; i < db->size(); i++) {
            graph.addNode();
        }

        for(int i = 0; i < db->size(); i++) {
            if(db->at(i)->deleted) continue;

            Vector<Pair<float, int>> nearest = findKNearest(i, maxConnections);

            for(int j = 0; j < nearest.length(); j++) {
                graph.addEdge(i, nearest[j].value);
            }
        }
    }

    Vector<int> greedySearch(const Vector<float>& query, int entryPoint, int k) {
        HashMap<int, bool> visited;
        MinHeap<Pair<float, int>> candidates(k);
        MinHeap<Pair<float, int>> results(k);
        
        float entrySim = Similarity::cosineSimilarity(query, db->at(entryPoint)->embedding);
        candidates.push(Pair<float, int>(-entrySim, entryPoint));
        results.push(Pair<float, int>(entrySim, entryPoint));
        visited.insert(entryPoint, true);
        
        while(candidates.size() > 0) {
            Pair<float, int> current = candidates.top();
            candidates.pop();
            
            float currentDist = -current.key;
            
            if(results.size() >= k && currentDist < results.top().key) {
                break;
            }
            
            Vector<int>& neighbors = graph.neighbors(current.value);
            
            for(int i = 0; i < neighbors.length(); i++) {
                int neighborId = neighbors[i];
                
                if(visited.contains(neighborId)) continue;

                visited.insert(neighborId, true);

                if(db->at(neighborId)->deleted) continue; // still marked visited so we don't loop back to it

                float neighborSim = Similarity::cosineSimilarity(query, db->at(neighborId)->embedding);

                if(results.size() < k) {
                    candidates.push(Pair<float, int>(-neighborSim, neighborId));
                    results.push(Pair<float, int>(neighborSim, neighborId));
                } else if(neighborSim > results.top().key) {
                    candidates.push(Pair<float, int>(-neighborSim, neighborId));
                    results.pop();
                    results.push(Pair<float, int>(neighborSim, neighborId));
                }
            }
        }
        
        Vector<Pair<float, int>> temp;
        while(results.size() > 0) {
            temp.push_back(results.top());
            results.pop();
        }
        
        Vector<int> resultIds;
        for(int i = temp.length() - 1; i >= 0; i--) {
            resultIds.push_back(temp[i].value);
        }
        
        return resultIds;
    }
    
    Vector<Pair<int, float>> search(const Vector<float>& query, int k, const MetadataFilter* filter = nullptr) {
        if(db->size() == 0) return Vector<Pair<int, float>>();

        int entryPoint = 0;
        while(entryPoint < db->size() && db->at(entryPoint)->deleted) entryPoint++;
        if(entryPoint >= db->size()) return Vector<Pair<int, float>>(); // everything deleted

        Vector<int> candidateIds = greedySearch(query, entryPoint, k * 2);

        MinHeap<Pair<float, int>> topK(k);

        for(int i = 0; i < candidateIds.length(); i++) {
            VectorItem* item = db->at(candidateIds[i]);
            if(item->deleted) continue; // defensive; greedySearch already filters these out
            if(!item->matchesFilter(filter)) continue;

            float sim = Similarity::cosineSimilarity(query, item->embedding);

            if(topK.size() < k) {
                topK.push(Pair<float, int>(sim, item->id));
            } else if(sim > topK.top().key) {
                topK.pop();
                topK.push(Pair<float, int>(sim, item->id));
            }
        }
        
        Vector<Pair<float, int>> temp;
        while(topK.size() > 0) {
            temp.push_back(topK.top());
            topK.pop();
        }
        
        Vector<Pair<int, float>> results;
        for(int i = temp.length() - 1; i >= 0; i--) {
            results.push_back(Pair<int, float>(temp[i].value, temp[i].key));
        }
        
        return results;
    }
    
    int getGraphSize() const {
        return graph.size();
    }
};

#endif
