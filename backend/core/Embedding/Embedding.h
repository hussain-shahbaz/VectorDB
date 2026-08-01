#ifndef EMBEDDING_H
#define EMBEDDING_H

#include<iostream>
#include <string>
#include<fstream>

// Fix Windows byte conflict
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <nlohmann/json.hpp>
#include "../../dataStructures/Vector.h"

using json = nlohmann::json;

#include <curl/curl.h>
class Embedding {
private:
    string apiKey;
    string apiUrl = "https://generativelanguage.googleapis.com/v1beta/models/gemini-embedding-001:embedContent";

    static size_t WriteData(void* ptr, size_t size, size_t nmemb, string* data) {
        data->append((char*)ptr, size * nmemb);
        return size * nmemb;
    }

public:
    Embedding(string key) : apiKey(key) {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }

    ~Embedding() {
        curl_global_cleanup();
    }

    Vector<float> getEmbedding(string text) {
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
            
            if (curl_easy_perform(curl) == CURLE_OK) {
                 cout << "Raw API Response: " << response << endl;

                auto j = json::parse(response);
                ofstream file("answer.json");
                if (file.is_open()) {
                    file << j;
                    file.close();
                }
                if (j.contains("embedding")) {
                    for (auto& val : j["embedding"]["values"]) {
                        result.push_back(val.get<float>());
                    }
                }
            }

            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
        }
        return result;
    }
};

#endif
