#ifndef KDANN_H
#define KDANN_H

#include "VectorDB.h"
#include "Similarity.h"
#include "../../dataStructures/Vector.h"
#include "../../dataStructures/Pair.h"
#include "../../dataStructures/Heap.h"
#include <cmath>

// KD-tree exact k-NN search. Unlike SearchEngine/GraphANN (which rank by
// cosine similarity, higher-is-better), KDANN ranks by Euclidean distance,
// lower-is-better — the axis-aligned pruning that makes a KD-tree worth
// building only works for a metric where "distance along one axis" bounds
// the full distance, which cosine similarity doesn't give you. Results are
// still returned as Pair<id, distance>, sorted ascending (nearest first).
class KDNode {
public:
    int id;
    KDNode* left;
    KDNode* right;
    KDNode(int nodeId) : id(nodeId), left(nullptr), right(nullptr) {}
};

class KDANN {
private:
    VectorDB* db;
    KDNode* root;
    int dim;

    void destroy(KDNode* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

    // In-place quicksort of ids[lo, hi) by their embedding's value along
    // `axis`, so build() can pick the median as the splitting node.
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

    KDNode* build(Vector<int>& ids, int lo, int hi, int depth) {
        if (lo >= hi) return nullptr;

        int axis = depth % dim;
        sortByAxis(ids, lo, hi, axis);
        int mid = lo + (hi - lo) / 2;

        KDNode* node = new KDNode(ids[mid]);
        node->left = build(ids, lo, mid, depth + 1);
        node->right = build(ids, mid + 1, hi, depth + 1);
        return node;
    }

    void searchNode(KDNode* node, const Vector<float>& query, int depth, MaxHeap<Pair<float, int>>& best, int k, const MetadataFilter* filter) {
        if (!node) return;

        VectorItem* item = db->get(node->id);
        if (!item) return;

        if (!item->deleted && item->matchesFilter(filter)) {
            float dist = Similarity::euclideanDistance(query, item->embedding);
            if (best.size() < k) {
                best.push(Pair<float, int>(dist, node->id));
            } else if (dist < best.top().key) {
                best.pop();
                best.push(Pair<float, int>(dist, node->id));
            }
        }

        int axis = depth % dim;
        float diff = query[axis] - item->embedding[axis];

        KDNode* nearSide = (diff < 0) ? node->left : node->right;
        KDNode* farSide = (diff < 0) ? node->right : node->left;

        searchNode(nearSide, query, depth + 1, best, k, filter);

        // The far subtree can only contain a closer point if the query is
        // within `worst kept distance` of the splitting plane.
        if (best.size() < k || fabs(diff) < best.top().key) {
            searchNode(farSide, query, depth + 1, best, k, filter);
        }
    }

public:
    KDANN(VectorDB* database) : db(database), root(nullptr), dim(0) {}
    ~KDANN() { destroy(root); }

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

        root = build(ids, 0, ids.length(), 0);
    }

    Vector<Pair<int, float>> search(const Vector<float>& query, int k, const MetadataFilter* filter = nullptr) {
        Vector<Pair<int, float>> results;
        if (!root || k <= 0) return results;

        MaxHeap<Pair<float, int>> best(k);
        searchNode(root, query, 0, best, k, filter);

        Vector<Pair<float, int>> temp;
        while (best.size() > 0) {
            temp.push_back(best.top());
            best.pop();
        }

        // temp is worst-first (descending distance); reverse for best-first.
        for (int i = temp.length() - 1; i >= 0; i--) {
            results.push_back(Pair<int, float>(temp[i].value, temp[i].key));
        }
        return results;
    }
};

#endif
