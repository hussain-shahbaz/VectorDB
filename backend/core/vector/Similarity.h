#ifndef SIMILARITY_H
#define SIMILARITY_H

#include "../../dataStructures/Vector.h"
#include <cmath>

class Similarity {
public:
    static float cosineSimilarity(const Vector<float>& a, const Vector<float>& b) {
        float dot = 0, magA = 0, magB = 0;
        for(int i = 0; i < a.length(); i++) {
            dot += a[i] * b[i];
            magA += a[i] * a[i];
            magB += b[i] * b[i];
        }
        return dot / (sqrt(magA) * sqrt(magB));
    }
    
    static float euclideanDistance(const Vector<float>& a, const Vector<float>& b) {
        float sum = 0;
        for(int i = 0; i < a.length(); i++) {
            float diff = a[i] - b[i];
            sum += diff * diff;
        }
        return sqrt(sum);
    }
};

#endif