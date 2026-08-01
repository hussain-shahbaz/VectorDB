#ifndef VECTORITEM_H
#define VECTORITEM_H

#include "../../dataStructures/Vector.h"
#include "../../dataStructures/HashMap.h"
#include <string>

using namespace std;

// A single required key=value constraint applied during search. Deliberately
// minimal — one exact-match key/value pair, not a general filter expression
// language — since that's what the project report's "filter by
// category/tag" use case actually needs.
struct MetadataFilter {
    string key;
    string value;
    MetadataFilter(const string& k, const string& v) : key(k), value(v) {}
};

class VectorItem{
public:
    int id;
    Vector<float> embedding;
    HashMap<string, string> metadata;
    bool deleted;

    VectorItem(): id(-1), deleted(false) {}
    VectorItem(int i,const Vector<float>& emb): id(i),embedding(emb), deleted(false) { }

    void setMetadata(const string& key, const string& value) {
        metadata.insert(key, value);
    }

    string* getMetadata(const string& key) {
        return metadata.find(key);
    }

    bool matchesFilter(const MetadataFilter* filter) {
        if (!filter) return true;
        string* val = metadata.find(filter->key);
        return val != nullptr && *val == filter->value;
    }
};

#endif
