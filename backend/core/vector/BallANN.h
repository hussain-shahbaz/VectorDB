#ifndef BALLANN_H
#define BALLANN_H

#include "VectorDB.h"
#include "Similarity.h"
#include "../../dataStructures/Vector.h"
#include "../../dataStructures/Pair.h"
#include "../../dataStructures/Heap.h"
#include <cmath>

// Ball-tree exact k-NN search: recursively partitions points into nested
// hyperspheres, pruned during search via the triangle inequality (a ball
// can't contain anything closer than distance-to-centroid minus radius).
// Like KDANN, ranks by Euclidean distance (lower is better), not cosine
// similarity — see the comment in KDANN.h for why.
class BallNode {
public:
    Vector<float> centroid;
    float radius;
    BallNode* left;
    BallNode* right;
    Vector<int> pointIds; // populated only on leaf nodes
    bool leaf;

    BallNode() : radius(0), left(nullptr), right(nullptr), leaf(false) {}
};

class BallANN {
private:
    VectorDB* db;
    BallNode* root;
    int dim;
    int leafSize;

    Vector<float> computeCentroid(Vector<int>& ids, int lo, int hi) {
        Vector<float> centroid;
        for (int d = 0; d < dim; d++) centroid.push_back(0.0f);

        for (int i = lo; i < hi; i++) {
            Vector<float>& emb = db->get(ids[i])->embedding;
            for (int d = 0; d < dim; d++) centroid[d] += emb[d];
        }
        int count = hi - lo;
        for (int d = 0; d < dim; d++) centroid[d] /= count;
        return centroid;
    }

    float maxDistToCentroid(Vector<int>& ids, int lo, int hi, const Vector<float>& centroid) {
        float maxD = 0;
        for (int i = lo; i < hi; i++) {
            float d = Similarity::euclideanDistance(centroid, db->get(ids[i])->embedding);
            if (d > maxD) maxD = d;
        }
        return maxD;
    }

    int findMaxSpreadAxis(Vector<int>& ids, int lo, int hi) {
        int bestAxis = 0;
        float bestSpread = -1;
        for (int d = 0; d < dim; d++) {
            float mn = db->get(ids[lo])->embedding[d];
            float mx = mn;
            for (int i = lo + 1; i < hi; i++) {
                float v = db->get(ids[i])->embedding[d];
                if (v < mn) mn = v;
                if (v > mx) mx = v;
            }
            float spread = mx - mn;
            if (spread > bestSpread) {
                bestSpread = spread;
                bestAxis = d;
            }
        }
        return bestAxis;
    }

    void sortByAxis(Vector<int>& ids, int lo, int hi, int axis) {
        if (hi - lo <= 1) return;
        int pivotIdx = lo + (hi - lo) / 2;
        float pivotVal = db->get(ids[pivotIdx])->embedding[axis];
        int i = lo, j = hi - 1;
        while (i <= j) {
            while (db->get(ids[i])->embedding[axis] < pivotVal) i++;
            while (db->get(ids[j])->embedding[axis] > pivotVal) j--;
            if (i <= j) {
                ids.swap(i, j);
                i++; j--;
            }
        }
        sortByAxis(ids, lo, j + 1, axis);
        sortByAxis(ids, i, hi, axis);
    }

    BallNode* build(Vector<int>& ids, int lo, int hi) {
        BallNode* node = new BallNode();
        node->centroid = computeCentroid(ids, lo, hi);
        node->radius = maxDistToCentroid(ids, lo, hi, node->centroid);

        if (hi - lo <= leafSize) {
            node->leaf = true;
            for (int i = lo; i < hi; i++) node->pointIds.push_back(ids[i]);
            return node;
        }

        int axis = findMaxSpreadAxis(ids, lo, hi);
        sortByAxis(ids, lo, hi, axis);
        int mid = lo + (hi - lo) / 2;

        node->left = build(ids, lo, mid);
        node->right = build(ids, mid, hi);
        return node;
    }

    void destroy(BallNode* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

    void searchNode(BallNode* node, const Vector<float>& query, MaxHeap<Pair<float, int>>& best, int k, const MetadataFilter* filter) {
        if (!node) return;

        float distToCentroid = Similarity::euclideanDistance(query, node->centroid);

        // Prune: nothing in this ball can beat the current worst kept.
        if (best.size() >= k && (distToCentroid - node->radius) > best.top().key) {
            return;
        }

        if (node->leaf) {
            for (int i = 0; i < node->pointIds.length(); i++) {
                int id = node->pointIds[i];
                VectorItem* item = db->get(id);
                if (!item || item->deleted || !item->matchesFilter(filter)) continue;

                float d = Similarity::euclideanDistance(query, item->embedding);
                if (best.size() < k) {
                    best.push(Pair<float, int>(d, id));
                } else if (d < best.top().key) {
                    best.pop();
                    best.push(Pair<float, int>(d, id));
                }
            }
            return;
        }

        // Visit whichever child's centroid is closer first — improves the
        // odds the far child gets pruned once `best` is full.
        float distLeft = Similarity::euclideanDistance(query, node->left->centroid);
        float distRight = Similarity::euclideanDistance(query, node->right->centroid);

        if (distLeft <= distRight) {
            searchNode(node->left, query, best, k, filter);
            searchNode(node->right, query, best, k, filter);
        } else {
            searchNode(node->right, query, best, k, filter);
            searchNode(node->left, query, best, k, filter);
        }
    }

public:
    BallANN(VectorDB* database, int leaf = 8) : db(database), root(nullptr), dim(0), leafSize(leaf) {}
    ~BallANN() { destroy(root); }

    void buildTree() {
        destroy(root);
        root = nullptr;
        if (db->size() == 0) return;

        dim = db->at(0)->embedding.length();

        Vector<int> ids;
        for (int i = 0; i < db->size(); i++) {
            if (!db->at(i)->deleted) ids.push_back(db->at(i)->id);
        }
        if (ids.length() == 0) return;

        root = build(ids, 0, ids.length());
    }

    Vector<Pair<int, float>> search(const Vector<float>& query, int k, const MetadataFilter* filter = nullptr) {
        Vector<Pair<int, float>> results;
        if (!root || k <= 0) return results;

        MaxHeap<Pair<float, int>> best(k);
        searchNode(root, query, best, k, filter);

        Vector<Pair<float, int>> temp;
        while (best.size() > 0) {
            temp.push_back(best.top());
            best.pop();
        }

        for (int i = temp.length() - 1; i >= 0; i--) {
            results.push_back(Pair<int, float>(temp[i].value, temp[i].key));
        }
        return results;
    }
};

#endif
