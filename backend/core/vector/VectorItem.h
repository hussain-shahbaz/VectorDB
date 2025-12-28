#ifndef VECTORITEM_H
#define VECTORITEM_H

#include "../../dataStructures/Vector.h"


class VectorItem{
public:
    int id;
    Vector<float> embedding;
    VectorItem(): id(-1){}
    VectorItem(int i,const Vector<float>& emb): id(i),embedding(emb){ }
};

#endif