#include <iostream>
#include "../dataStructures/Huffman.h"

using namespace std;

void testBasicEncoding() {
    cout << "=== Test 1: Basic Encoding/Decoding ===" << endl;
    Huffman huffman;
    
    Vector<unsigned char> data;
    const char* text = "AAAAAABBBCCDE";
    for(int i = 0; text[i] != '\0'; i++){
        data.push_back(text[i]);
    }
    
    int originalSize = data.length();
    cout << "Original text: " << text << endl;
    cout << "Original size: " << originalSize << " bytes" << endl;
    
    Vector<unsigned char> encoded = huffman.encode(data);
    cout << "Compressed size: " << encoded.length() << " bytes" << endl;
    cout << "Compression ratio: " << huffman.getCompressionRatio(originalSize, encoded.length()) << endl;
    cout << endl;
    
    huffman.printCodeTable();
    cout << endl;
    
    Vector<unsigned char> decoded = huffman.decode(encoded, originalSize);
    
    cout << "Decoded text: ";
    for(int i = 0; i < decoded.length(); i++){
        cout << (char)decoded[i];
    }
    cout << endl;
    
    bool match = true;
    if(decoded.length() != data.length()){
        match = false;
    } else {
        for(int i = 0; i < data.length(); i++){
            if(data[i] != decoded[i]){
                match = false;
                break;
            }
        }
    }
    
    cout << "Match original: " << (match ? "YES" : "NO") << endl;
    cout << endl;
}

void testSingleCharacter() {
    cout << "=== Test 2: Single Character ===" << endl;
    Huffman huffman;
    
    Vector<unsigned char> data;
    for(int i = 0; i < 10; i++){
        data.push_back('A');
    }
    
    cout << "Data: AAAAAAAAAA" << endl;
    cout << "Original size: " << data.length() << " bytes" << endl;
    
    Vector<unsigned char> encoded = huffman.encode(data);
    cout << "Compressed size: " << encoded.length() << " bytes" << endl;
    
    Vector<unsigned char> decoded = huffman.decode(encoded, data.length());
    
    bool match = true;
    for(int i = 0; i < data.length(); i++){
        if(data[i] != decoded[i]){
            match = false;
            break;
        }
    }
    
    cout << "Match: " << (match ? "YES" : "NO") << endl;
    cout << endl;
}

void testLongText() {
    cout << "=== Test 3: Longer Text ===" << endl;
    Huffman huffman;
    
    Vector<unsigned char> data;
    const char* text = "this is a test of huffman encoding and decoding with longer text to see compression";
    for(int i = 0; text[i] != '\0'; i++){
        data.push_back(text[i]);
    }
    
    int originalSize = data.length();
    cout << "Original size: " << originalSize << " bytes" << endl;
    
    Vector<unsigned char> encoded = huffman.encode(data);
    cout << "Compressed size: " << encoded.length() << " bytes" << endl;
    cout << "Compression ratio: " << huffman.getCompressionRatio(originalSize, encoded.length()) << endl;
    
    Vector<unsigned char> decoded = huffman.decode(encoded, originalSize);
    
    bool match = (decoded.length() == data.length());
    if(match){
        for(int i = 0; i < data.length(); i++){
            if(data[i] != decoded[i]){
                match = false;
                break;
            }
        }
    }
    
    cout << "Decoded correctly: " << (match ? "YES" : "NO") << endl;
    cout << endl;
}

void testBinaryData() {
    cout << "=== Test 4: Binary Data ===" << endl;
    Huffman huffman;
    
    Vector<unsigned char> data;
    for(int i = 0; i < 256; i++){
        data.push_back((unsigned char)i);
    }
    
    int originalSize = data.length();
    cout << "Original size (all byte values): " << originalSize << " bytes" << endl;
    
    Vector<unsigned char> encoded = huffman.encode(data);
    cout << "Compressed size: " << encoded.length() << " bytes" << endl;
    
    Vector<unsigned char> decoded = huffman.decode(encoded, originalSize);
    
    bool match = (decoded.length() == data.length());
    if(match){
        for(int i = 0; i < data.length(); i++){
            if(data[i] != decoded[i]){
                match = false;
                cout << "Mismatch at index " << i << endl;
                break;
            }
        }
    }
    
    cout << "Decoded correctly: " << (match ? "YES" : "NO") << endl;
    cout << endl;
}

void testRepeatedPattern() {
    cout << "=== Test 5: Repeated Pattern ===" << endl;
    Huffman huffman;
    
    Vector<unsigned char> data;
    const char* pattern = "ABC";
    for(int i = 0; i < 20; i++){
        for(int j = 0; pattern[j] != '\0'; j++){
            data.push_back(pattern[j]);
        }
    }
    
    int originalSize = data.length();
    cout << "Pattern: ABC repeated 20 times" << endl;
    cout << "Original size: " << originalSize << " bytes" << endl;
    
    Vector<unsigned char> encoded = huffman.encode(data);
    cout << "Compressed size: " << encoded.length() << " bytes" << endl;
    cout << "Compression ratio: " << huffman.getCompressionRatio(originalSize, encoded.length()) << endl;
    
    Vector<unsigned char> decoded = huffman.decode(encoded, originalSize);
    
    bool match = (decoded.length() == data.length());
    if(match){
        for(int i = 0; i < data.length(); i++){
            if(data[i] != decoded[i]){
                match = false;
                break;
            }
        }
    }
    
    cout << "Decoded correctly: " << (match ? "YES" : "NO") << endl;
    cout << endl;
}

void testEmptyData() {
    cout << "=== Test 6: Empty Data ===" << endl;
    Huffman huffman;
    
    Vector<unsigned char> data;
    
    cout << "Original size: 0 bytes" << endl;
    
    Vector<unsigned char> encoded = huffman.encode(data);
    cout << "Compressed size: " << encoded.length() << " bytes (expected: 0)" << endl;
    
    Vector<unsigned char> decoded = huffman.decode(encoded, 0);
    cout << "Decoded size: " << decoded.length() << " bytes (expected: 0)" << endl;
    cout << endl;
}

void testTreeSerializationCrossObject() {
    cout << "=== Test 7: Tree Serialization (decode with a different object) ===" << endl;

    Huffman encoder;
    Vector<unsigned char> data;
    const char* text = "the quick brown fox jumps over the lazy dog";
    for (int i = 0; text[i] != '\0'; i++) data.push_back(text[i]);

    int originalSize = data.length();
    Vector<unsigned char> encoded = encoder.encode(data);
    Vector<unsigned char> treeBytes = encoder.serializeTree();

    cout << "Serialized tree: " << treeBytes.length() << " bytes" << endl;

    // Simulates loading from disk in a fresh process: a brand new Huffman
    // object, with no relation to `encoder`, decoding via a loaded tree.
    Huffman decoder;
    decoder.loadTree(treeBytes);
    Vector<unsigned char> decoded = decoder.decode(encoded, originalSize);

    bool match = decoded.length() == data.length();
    for (int i = 0; match && i < data.length(); i++) {
        if (decoded[i] != data[i]) match = false;
    }
    cout << (match ? "PASS" : "FAIL") << ": cross-object decode via serialized tree reproduces the original text" << endl;
    cout << endl;
}

void testTreeSerializationSingleSymbol() {
    cout << "=== Test 8: Tree Serialization (single-symbol edge case) ===" << endl;

    Huffman encoder;
    Vector<unsigned char> data;
    for (int i = 0; i < 20; i++) data.push_back('Z'); // only one distinct symbol

    int originalSize = data.length();
    Vector<unsigned char> encoded = encoder.encode(data);
    Vector<unsigned char> treeBytes = encoder.serializeTree();

    Huffman decoder;
    decoder.loadTree(treeBytes);
    Vector<unsigned char> decoded = decoder.decode(encoded, originalSize);

    bool match = decoded.length() == data.length();
    for (int i = 0; match && i < data.length(); i++) {
        if (decoded[i] != data[i]) match = false;
    }
    // The single-symbol tree has a root with only a left child — the
    // regression this test guards against is the null-right-child case
    // desyncing the deserializer.
    cout << (match ? "PASS" : "FAIL") << ": single-symbol tree (root with only a left child) round-trips correctly" << endl;
    cout << endl;
}

int main() {
    cout << "======================================" << endl;
    cout << "    Huffman Encoding Test Suite" << endl;
    cout << "======================================" << endl;
    cout << endl;
    
    testBasicEncoding();
    testSingleCharacter();
    testLongText();
    testBinaryData();
    testRepeatedPattern();
    testEmptyData();
    testTreeSerializationCrossObject();
    testTreeSerializationSingleSymbol();

    cout << "======================================" << endl;
    cout << "    All Tests Complete" << endl;
    cout << "======================================" << endl;
    
    return 0;
}
