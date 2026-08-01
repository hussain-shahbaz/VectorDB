#ifndef HUFFMANCOMPRESSION_H
#define HUFFMANCOMPRESSION_H

#include "../../dataStructures/Huffman.h"
#include "../../dataStructures/Vector.h"

// Converts between Vector<float> and the raw byte stream Huffman.h operates
// on, and drives encode/decode through a caller-supplied Huffman instance
// (the tree it builds is what makes decode() work — see Huffman.h).
class HuffmanCompression {
public:
    static Vector<unsigned char> floatsToBytes(const Vector<float>& floats) {
        Vector<unsigned char> bytes;
        for (int i = 0; i < floats.length(); i++) {
            float f = floats[i];
            unsigned char* p = reinterpret_cast<unsigned char*>(&f);
            for (int b = 0; b < (int)sizeof(float); b++) bytes.push_back(p[b]);
        }
        return bytes;
    }

    static Vector<float> bytesToFloats(const Vector<unsigned char>& bytes) {
        Vector<float> floats;
        for (int i = 0; i + (int)sizeof(float) - 1 < bytes.length(); i += sizeof(float)) {
            float f;
            unsigned char* p = reinterpret_cast<unsigned char*>(&f);
            for (int b = 0; b < (int)sizeof(float); b++) p[b] = bytes[i + b];
            floats.push_back(f);
        }
        return floats;
    }

    static Vector<unsigned char> compress(Huffman& huffman, const Vector<float>& floats) {
        return huffman.encode(floatsToBytes(floats));
    }

    static Vector<float> decompress(Huffman& huffman, const Vector<unsigned char>& compressed, int originalByteLength) {
        return bytesToFloats(huffman.decode(compressed, originalByteLength));
    }
};

#endif
