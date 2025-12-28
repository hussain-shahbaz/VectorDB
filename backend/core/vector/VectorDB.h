#ifndef VECTORDB_H
#define VECTORDB_H

#include "VectorItem.h"
#include "../../dataStructures/Vector.h"

class VectorDB{
private:
    Vector<VectorItem*> vectors;
    int nextId;
public:
    VectorDB(): nextId(0) {}

    ~VectorDB() {
        for(int i = 0; i < vectors.length(); i++) {
            delete vectors[i];
        }
    }

    int insert(const Vector<float>& embedding){
        VectorItem* item = new VectorItem(nextId++,embedding);
        vectors.push_back(item);
        return nextId;
    }

    VectorItem* get(int id){
        for(int i=0;i<vectors.lenght();i++){
            if(vectors[i]->id == id) return vectors[i];
        }
        return nullptr;
    }

    int size() const {
        return vectors.lenght();
    }

    VectorItem* at(const int index){
        return vectors[index];
    }

};

#endif