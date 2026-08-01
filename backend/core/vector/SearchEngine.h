#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include "VectorDB.h"
#include "Similarity.h"
#include "../../dataStructures/Heap.h"
#include "../../dataStructures/Pair.h"
class SearchEngine {
private:
    VectorDB* db;
public:
    SearchEngine(VectorDB* database): db(database) {}

    Vector<Pair<int,float>> search(const Vector<float>& query, int k){ // KNN
        
        MinHeap<Pair<float,int>> topK(k);

        for(int i=0;i<db->size();i++){
            VectorItem* item = db->at(i);

            float similarity = Similarity::cosineSimilarity(query,item->embedding);

            if(topK.size() < k){
                topK.push(Pair<float,int>(similarity,item->id));
            }else if(similarity > topK.top().key){
                topK.pop();
                topK.push(Pair<float,int>(similarity,item->id));
            }
        }

        Vector<Pair<int,float>> results;
        Vector<Pair<float,int>> temp;
        
        while(topK.size() > 0){
            auto top = topK.top();
            temp.push_back(top);
            topK.pop();
        }

        for(int i=temp.length()-1;i>=0;i--){
            results.push_back(Pair<int,float>(temp[i].value, temp[i].key));
        }

        return results;
    }
};

#endif