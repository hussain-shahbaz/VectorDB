#ifndef HUFFMAN_H
#define HUFFMAN_H

#include<iostream>
#include"HashMap.h"
#include"Heap.h"
#include"Vector.h"
using namespace std;

class HuffmanNode{
public:
    unsigned char ch;
    int frequency;
    HuffmanNode *left,*right;

    HuffmanNode(unsigned char c,int freq,HuffmanNode* l =nullptr,HuffmanNode* r = nullptr){
        this->ch = c;
        this->frequency = freq;
        this->left = l;
        this->right = r;
    }

    bool operator<(const HuffmanNode& other) const {
        return this->frequency < other.frequency;
    }

    bool operator>(const HuffmanNode& other) const {
        return this->frequency > other.frequency;
    }

    bool operator==(const HuffmanNode& other) const {
        return this->frequency == other.frequency;
    }

    bool isLeaf() const {
        return left == nullptr && right == nullptr;
    }
};

class Huffman{
private:
    HuffmanNode* root;
    HashMap<unsigned char, Vector<bool>*> codeTable;

    void destroyTree(HuffmanNode* node){
        if(!node) return;
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }

    void buildFrequencyMap(const Vector<unsigned char>& data, HashMap<unsigned char,int>& freq){
        for(int i = 0; i < data.length(); i++){
            unsigned char byte = data[i];
            int* count = freq.find(byte);
            if(count == nullptr){
                freq.insert(byte, 1);
            } else {
                (*count)++;
            }
        }
    }

    HuffmanNode* buildTree(HashMap<unsigned char,int>& freq){
        MinHeap<HuffmanNode*> pq(256);

        for(int i = 0; i < freq.capacity; i++){
            if(freq.table[i].state == OCCUPIED){
                HuffmanNode* node = new HuffmanNode(freq.table[i].key, freq.table[i].value);
                pq.push(node);
            }
        }

        if(pq.size() == 1){
            HuffmanNode* only = pq.top(); 
            pq.pop();
            HuffmanNode* newRoot = new HuffmanNode('\0', only->frequency, only, nullptr);
            return newRoot;
        }

        while(pq.size() > 1){
            HuffmanNode* left = pq.top(); pq.pop();
            HuffmanNode* right = pq.top(); pq.pop();

            HuffmanNode* merged = new HuffmanNode('\0', left->frequency + right->frequency, left, right);
            pq.push(merged);
        }

        return pq.top();
    }

    void generateCodes(HuffmanNode* node, Vector<bool>& currentCode){
        if(!node) return;

        if(node->isLeaf()){
            Vector<bool>* code = new Vector<bool>();
            if(currentCode.length() == 0){
                code->push_back(false);
            } else {
                for(int i = 0; i < currentCode.length(); i++){
                    code->push_back(currentCode[i]);
                }
            }
            codeTable.insert(node->ch, code);
            return;
        }

        currentCode.push_back(false);
        generateCodes(node->left, currentCode);
        currentCode.pop_back();

        currentCode.push_back(true);
        generateCodes(node->right, currentCode);
        currentCode.pop_back();
    }

    Vector<unsigned char> packBits(const Vector<bool>& bits){
        Vector<unsigned char> bytes;
        for(int i = 0; i < bits.length(); i += 8){
            unsigned char byte = 0;
            for(int j = 0; j < 8 && (i + j) < bits.length(); j++){
                if(bits[i + j]){    
                    byte |= (1 << (7 - j));
                }
            }
            bytes.push_back(byte);
        }
        return bytes;
    }

    // Tree wire format, one byte per node plus one payload byte per leaf:
    //   0            -> null child
    //   1, <byte>     -> leaf holding <byte>
    //   2             -> internal node, followed by serialized left then right
    // A 3-way marker (rather than "just omit null children") is required
    // because a single-symbol input produces a root with a left child and
    // no right child — omitting absent children would desync the reader.
    void serializeTreeNode(HuffmanNode* node, Vector<unsigned char>& out){
        if(!node){ out.push_back(0); return; }
        if(node->isLeaf()){
            out.push_back(1);
            out.push_back(node->ch);
            return;
        }
        out.push_back(2);
        serializeTreeNode(node->left, out);
        serializeTreeNode(node->right, out);
    }

    HuffmanNode* deserializeTreeNode(const Vector<unsigned char>& data, int& pos){
        if(pos >= data.length()) return nullptr;
        unsigned char marker = data[pos++];
        if(marker == 0) return nullptr;
        if(marker == 1){
            unsigned char ch = data[pos++];
            return new HuffmanNode(ch, 0);
        }
        HuffmanNode* node = new HuffmanNode('\0', 0);
        node->left = deserializeTreeNode(data, pos);
        node->right = deserializeTreeNode(data, pos);
        return node;
    }

    Vector<bool> unpackBits(const Vector<unsigned char>& bytes){
        Vector<bool> bits;
        for(int i = 0; i < bytes.length(); i++){
            for(int j = 0; j < 8; j++){
                bits.push_back((bytes[i] >> (7 - j)) & 1);
            }
        }
        return bits;
    }

public:
    Huffman() : root(nullptr), codeTable(256) {}

    ~Huffman(){
        destroyTree(root);
        for(int i = 0; i < codeTable.capacity; i++){
            if(codeTable.table[i].state == OCCUPIED){
                delete codeTable.table[i].value;
            }
        }
    }

    Vector<unsigned char> encode(const Vector<unsigned char>& data){
        if(data.length() == 0) return Vector<unsigned char>();

        HashMap<unsigned char,int> freq(256);
        buildFrequencyMap(data, freq);

        root = buildTree(freq);

        Vector<bool> emptyCode;
        generateCodes(root, emptyCode);

        Vector<bool> encodedBits;
        for(int i = 0; i < data.length(); i++){
            Vector<bool>** code = codeTable.find(data[i]);
            if(code != nullptr && *code != nullptr){
                Vector<bool>* codeVec = *code;
                for(int j = 0; j < codeVec->length(); j++){
                    encodedBits.push_back((*codeVec)[j]);
                }
            }
        }

        return packBits(encodedBits);
    }

    // decode() only ever walks `root`, so a tree built by a *different*
    // Huffman object (e.g. after loading from disk in a fresh process) can
    // still decode data encoded here, as long as it's restored via these.
    Vector<unsigned char> serializeTree(){
        Vector<unsigned char> out;
        serializeTreeNode(root, out);
        return out;
    }

    void loadTree(const Vector<unsigned char>& treeData){
        destroyTree(root);
        int pos = 0;
        root = deserializeTreeNode(treeData, pos);
    }

    Vector<unsigned char> decode(const Vector<unsigned char>& encoded, int originalSize){
        if(root == nullptr) return Vector<unsigned char>();

        Vector<bool> bits = unpackBits(encoded);
        Vector<unsigned char> decoded;

        HuffmanNode* current = root;

        for(int i = 0; i < bits.length() && decoded.length() < originalSize; i++){
            if(bits[i]){
                current = current->right;
            } else {
                current = current->left;
            }

            if(current && current->isLeaf()){
                decoded.push_back(current->ch);
                current = root;
            }
        }

        return decoded;
    }

    void printCodeTable(){
        cout << "Huffman Code Table:" << endl;
        for(int i = 0; i < codeTable.capacity; i++){
            if(codeTable.table[i].state == OCCUPIED){
                unsigned char ch = codeTable.table[i].key;
                Vector<bool>* code = codeTable.table[i].value;
                cout << (ch >= 32 && ch <= 126 ? (char)ch : '?') << " (" << (int)ch << "): ";
                for(int j = 0; j < code->length(); j++){
                    cout << ((*code)[j] ? '1' : '0');
                }
                cout << endl;
            }
        }
    }

    double getCompressionRatio(int originalSize, int compressedSize){
        return (double)compressedSize / originalSize;
    }
};

#endif