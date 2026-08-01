#ifndef EMBEDDING_CACHE_H
#define EMBEDDING_CACHE_H

#include <fstream>
#include <sstream>
#include <string>
#include "dataStructures/Vector.h"

// Simple disk-backed cache mapping text -> embedding, keyed by exact string
// match. Not a HashMap because HashMap<K,V> here only hashes int-convertible
// keys (see build-plan.md); a linear scan is fine at demo-corpus scale.
// Keeps repeat runs of main.cpp from re-spending live API quota on the same
// seed words.
class EmbeddingCache {
private:
    std::string path;
    Vector<std::string> words;
    Vector<Vector<float>> embeddings;

    static Vector<float> parseValues(const std::string& csv) {
        Vector<float> values;
        std::stringstream ss(csv);
        std::string token;
        while (std::getline(ss, token, ',')) {
            if (!token.empty()) values.push_back(std::stof(token));
        }
        return values;
    }

    static std::string toCsv(const Vector<float>& v) {
        std::stringstream ss;
        for (int i = 0; i < v.length(); i++) {
            if (i) ss << ",";
            ss << v[i];
        }
        return ss.str();
    }

public:
    explicit EmbeddingCache(const std::string& cachePath) : path(cachePath) {
        std::ifstream file(path);
        std::string line;
        while (std::getline(file, line)) {
            size_t tab = line.find('\t');
            if (tab == std::string::npos) continue;
            words.push_back(line.substr(0, tab));
            embeddings.push_back(parseValues(line.substr(tab + 1)));
        }
    }

    bool find(const std::string& word, Vector<float>& out) const {
        for (int i = 0; i < words.length(); i++) {
            if (words[i] == word) {
                out = embeddings[i];
                return true;
            }
        }
        return false;
    }

    void put(const std::string& word, const Vector<float>& embedding) {
        words.push_back(word);
        embeddings.push_back(embedding);

        std::ofstream file(path, std::ios::app);
        file << word << "\t" << toCsv(embedding) << "\n";
    }
};

#endif
