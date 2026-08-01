#ifndef VECTORDB_H
#define VECTORDB_H

#include "VectorItem.h"
#include "../../dataStructures/Vector.h"
#include "../../dataStructures/Huffman.h"
#include "../storage/BtreeIndex.h"
#include "../storage/HuffmanCompression.h"
#include <cstdint>
#include <fstream>
#include <string>

using namespace std;

// On-disk format (little-endian, native to whatever machine writes it —
// portability across architectures isn't a goal here):
//   uint32 magic ("VDB!" as 0x56444221)
//   uint32 version
//   uint32 count           (active/non-deleted vectors only — save() compacts)
//   uint32 dim
//   int32  ids[count]
//   uint32 huffmanTreeLength, huffmanTreeBytes[...]
//   uint32 originalByteLength, compressedByteLength, compressedBytes[...]
// All vectors are Huffman-compressed as a single concatenated block (one
// shared tree gives much better compression than per-vector trees, and
// matches how Huffman.h's encode()/decode() are actually meant to be used —
// see the comment on BtreeIndex for what that costs us). Loading is eager:
// the whole block is decompressed at once, then the id->position BtreeIndex
// is rebuilt from the id table. That means the on-disk B-tree "index" here
// is really just the flat id table — genuinely offset-addressable, direct
// random access into the *compressed* bytes for a single vector isn't
// possible without giving each vector its own Huffman tree, which is worth
// revisiting if this ever needs to scale past what fits comfortably in
// memory. The in-memory BtreeIndex built from that table is real and used
// for all id lookups, on-disk or not.
static const uint32_t VDB_MAGIC = 0x56444221;
static const uint32_t VDB_VERSION = 1;

class VectorDB{
private:
    Vector<VectorItem*> vectors;
    BtreeIndex idIndex;
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
        idIndex.insert(item->id, vectors.length() - 1);
        return item->id;
    }

    VectorItem* get(int id){
        int pos;
        if(!idIndex.find(id, pos)) return nullptr;
        return vectors[pos];
    }

    // Soft delete: marks the item as deleted rather than physically removing
    // it, so ids stay stable (positions never shift, which the graph/tree
    // indexes rely on). Search classes skip deleted items.
    bool remove(int id){
        VectorItem* item = get(id);
        if(!item) return false;
        item->deleted = true;
        return true;
    }

    bool update(int id, const Vector<float>& embedding){
        VectorItem* item = get(id);
        if(!item) return false;
        item->embedding = embedding;
        return true;
    }

    int size() const {
        return vectors.length();
    }

    // Count of items not soft-deleted. size() stays the raw storage count
    // (positions never shift) — use this for a "how many can I actually get
    // back from a search" figure.
    int activeSize() const {
        int count = 0;
        for(int i = 0; i < vectors.length(); i++){
            if(!vectors[i]->deleted) count++;
        }
        return count;
    }

    VectorItem* at(const int index){
        return vectors[index];
    }

    bool save(const string& path){
        Vector<VectorItem*> active;
        for(int i = 0; i < vectors.length(); i++){
            if(!vectors[i]->deleted) active.push_back(vectors[i]);
        }

        int count = active.length();
        int dim = count > 0 ? active[0]->embedding.length() : 0;

        Vector<float> allFloats;
        for(int i = 0; i < count; i++){
            for(int d = 0; d < dim; d++) allFloats.push_back(active[i]->embedding[d]);
        }

        Huffman huffman;
        Vector<unsigned char> compressed = HuffmanCompression::compress(huffman, allFloats);
        Vector<unsigned char> treeBytes = huffman.serializeTree();

        ofstream out(path, ios::binary);
        if(!out.is_open()) return false;

        uint32_t magic = VDB_MAGIC, version = VDB_VERSION;
        uint32_t countU = (uint32_t)count, dimU = (uint32_t)dim;
        out.write((const char*)&magic, sizeof(magic));
        out.write((const char*)&version, sizeof(version));
        out.write((const char*)&countU, sizeof(countU));
        out.write((const char*)&dimU, sizeof(dimU));

        for(int i = 0; i < count; i++){
            int32_t id = active[i]->id;
            out.write((const char*)&id, sizeof(id));
        }

        uint32_t treeLen = (uint32_t)treeBytes.length();
        out.write((const char*)&treeLen, sizeof(treeLen));
        if(treeLen > 0) out.write((const char*)treeBytes.data(), treeLen);

        uint32_t originalLen = (uint32_t)(count * dim * sizeof(float));
        uint32_t compressedLen = (uint32_t)compressed.length();
        out.write((const char*)&originalLen, sizeof(originalLen));
        out.write((const char*)&compressedLen, sizeof(compressedLen));
        if(compressedLen > 0) out.write((const char*)compressed.data(), compressedLen);

        return out.good();
    }

    bool load(const string& path){
        ifstream in(path, ios::binary);
        if(!in.is_open()) return false;

        uint32_t magic = 0, version = 0, countU = 0, dimU = 0;
        in.read((char*)&magic, sizeof(magic));
        if(!in.good() || magic != VDB_MAGIC) return false;
        in.read((char*)&version, sizeof(version));
        if(version != VDB_VERSION) return false;
        in.read((char*)&countU, sizeof(countU));
        in.read((char*)&dimU, sizeof(dimU));
        int count = (int)countU, dim = (int)dimU;

        Vector<int> ids;
        for(int i = 0; i < count; i++){
            int32_t id = 0;
            in.read((char*)&id, sizeof(id));
            ids.push_back(id);
        }

        uint32_t treeLen = 0;
        in.read((char*)&treeLen, sizeof(treeLen));
        Vector<unsigned char> treeBytes;
        treeBytes.resize((int)treeLen);
        if(treeLen > 0) in.read((char*)treeBytes.data(), treeLen);

        uint32_t originalLen = 0, compressedLen = 0;
        in.read((char*)&originalLen, sizeof(originalLen));
        in.read((char*)&compressedLen, sizeof(compressedLen));
        Vector<unsigned char> compressed;
        compressed.resize((int)compressedLen);
        if(compressedLen > 0) in.read((char*)compressed.data(), compressedLen);

        if(!in.good() && !in.eof()) return false;

        Huffman huffman;
        huffman.loadTree(treeBytes);
        Vector<float> allFloats = HuffmanCompression::decompress(huffman, compressed, (int)originalLen);

        for(int i = 0; i < vectors.length(); i++) delete vectors[i];
        vectors.clear();
        idIndex.clear();
        nextId = 0;

        for(int i = 0; i < count; i++){
            Vector<float> emb;
            for(int d = 0; d < dim; d++) emb.push_back(allFloats[i * dim + d]);

            VectorItem* item = new VectorItem(ids[i], emb);
            vectors.push_back(item);
            idIndex.insert(ids[i], i);
            if(ids[i] >= nextId) nextId = ids[i] + 1;
        }

        return true;
    }

};

#endif
