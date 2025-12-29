#include "../core/geminiEmbedding/Embedding.h"
#include <iostream>
#include <cstdlib>

using namespace std;

int main() {
    const char* env_key = getenv("GEMINI_API_KEY");
    env_key = "REDACTED_GEMINI_API_KEY";
    if (!env_key) {
        cerr << "Error: GEMINI_API_KEY environment variable not set." << endl;
        return 1;
    }

    string apiKey = string(env_key);
    Embedding api(apiKey);

    cout << "Requesting embedding for: 'happy'..." << endl;
    Vector<float> embedding = api.getEmbedding("happy");

    if (embedding.empty()) {
        cerr << "Error: Received empty embedding. Check API key and internet connection." << endl;
        return 1;
    }

    cout << "Success!" << endl;
    cout << "Vector Size: " << embedding.lenght()<< " dimensions" << endl;
    

    for (size_t i = 0; i < embedding.lenght(); ++i) {
        cout << i+1<<": "<< embedding[i] << (i == 4 ? "" : ", ");
    }
    cout << "..." << endl;

    return 0;
}
