#ifndef EMBEDDING_H
#define EMBEDDING_H

#include<iostream>
#include <string>
#include<fstream>

#include <nlohmann/json.hpp>
#include "../../dataStructures/Vector.h"
#include "../../dataStructures/HashMap.h"
#include "../../dataStructures/List.h"

using json = nlohmann::json;
using namespace std;

#include <curl/curl.h>

// In-memory, process-lifetime cache of text -> embedding, with LRU eviction
// once maxCacheSize is exceeded (a HashMap for O(1) lookup, plus a List
// tracking usage order so the least-recently-used entry can be evicted).
// This is separate from — and complementary to — the disk-backed
// EmbeddingCache used by main.cpp: this one avoids redundant API calls
// *within* a single run, EmbeddingCache avoids them *across* runs.
class Embedding {
private:
    string apiKey;
    string apiUrl = "https://generativelanguage.googleapis.com/v1beta/models/gemini-embedding-001:embedContent";

    HashMap<string, Vector<float>> cache;
    List<string> cacheOrder; // front = least recently used, back = most recently used
    int maxCacheSize;

    void touchCacheKey(const string& key) {
        cacheOrder.remove(key);
        cacheOrder.insertAtEnd(key);
    }

    void cacheInsert(const string& key, const Vector<float>& value) {
        if (cache.find(key) != nullptr) {
            cache.insert(key, value);
            touchCacheKey(key);
            return;
        }
        if (cacheOrder.size() >= maxCacheSize) {
            string lru = cacheOrder.getHead()->data;
            cacheOrder.remove(lru);
            cache.remove(lru);
        }
        cache.insert(key, value);
        cacheOrder.insertAtEnd(key);
    }

    static size_t WriteData(void* ptr, size_t size, size_t nmemb, string* data) {
        data->append((char*)ptr, size * nmemb);
        return size * nmemb;
    }

public:
    Embedding(string key, int cacheSize = 100) : apiKey(key), maxCacheSize(cacheSize) {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    ~Embedding() {
        curl_global_cleanup();
    }

    int getCacheSize() const {
        return cacheOrder.size();
    }

    Vector<float> getEmbedding(string text) {
        Vector<float>* cached = cache.find(text);
        if (cached != nullptr) {
            touchCacheKey(text);
            return *cached;
        }

        CURL* curl = curl_easy_init();
        string response;
        Vector<float> result;

        if (curl) {
            json body = {
                {"model", "models/gemini-embedding-001"},
                {"content", {{"parts", {{{"text", text}}}}}},
                {"output_dimensionality", 128}
            };

            string jsonStr = body.dump();
            
            struct curl_slist* headers = NULL;
            headers = curl_slist_append(headers, "Content-Type: application/json");
            headers = curl_slist_append(headers, ("x-goog-api-key: " + apiKey).c_str());
            headers = curl_slist_append(headers, "Host: generativelanguage.googleapis.com");
            headers = curl_slist_append(headers, "Connection: keep-alive");
            headers = curl_slist_append(headers, "Accept: application/json");
            
            curl_easy_setopt(curl, CURLOPT_URL, apiUrl.c_str());
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonStr.c_str());
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteData);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            // Without these, a stalled connection (bad network, a request
            // that never gets a response) hangs curl_easy_perform() forever
            // — fatal for a bulk loader making thousands of sequential
            // calls unattended.
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

            CURLcode res = curl_easy_perform(curl);
            if (res == CURLE_OK) {
                try {
                    auto j = json::parse(response);
                    if (j.contains("embedding")) {
                        for (auto& val : j["embedding"]["values"]) {
                            result.push_back(val.get<float>());
                        }
                    } else {
                        cerr << "Embedding API error for \"" << text << "\": " << response << endl;
                    }
                } catch (const json::parse_error& e) {
                    cerr << "Embedding API returned unparsable response for \"" << text << "\": " << e.what() << endl;
                }
            } else {
                cerr << "Embedding request failed for \"" << text << "\": " << curl_easy_strerror(res) << endl;
            }

            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        }

        if (!result.empty()) {
            cacheInsert(text, result);
        }
        return result;
    }
};

#endif
