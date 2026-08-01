#define _HAS_STD_BYTE 0  // Fix for Windows header conflict with std::byte

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <string>

#include "../core/vector/VectorDB.h"
#include "../core/vector/VectorItem.h"
#include "../core/vector/SearchEngine.h"
#include "../core/vector/GraphANN.h"
#include "../core/vector/Similarity.h"
#include "../core/embedding/Embedding.h"

using namespace std;

Vector<float> normalizeVector(const Vector<float>& vec) {
    if(vec.length() == 0){
        cout << "WARNING: Empty embedding received. Skipping normalization.\n";
        return vec;
    }

    float mag = 0;

    for(int i = 0; i < vec.length(); i++)
        mag += vec[i] * vec[i];

    if(mag == 0){
        cout << "WARNING: Zero magnitude embedding. Returning original.\n";
        return vec;
    }

    mag = sqrt(mag);

    Vector<float> norm;
    for(int i = 0; i < vec.length(); i++)
        norm.push_back(vec[i] / mag);

    return norm;
}

int main(){
    srand(time(0));

    cout << "\n====================================\n";
    cout << " LARGE SCALE ANN vs BRUTE FORCE TEST\n";
    cout << "====================================\n";

    //------------------------------------
    // Load API
    //------------------------------------
    const char* env_key = getenv("OPENAI_API_KEY");
    
    env_key = "REDACTED_GEMINI_API_KEY";

    if(!env_key){
        cout << "ERROR: OPENAI_API_KEY not set!\n";
        return 0;
    }

    string apiKey = string(env_key);
    Embedding api(apiKey);

    //------------------------------------
    // Word List
    //------------------------------------
    string words[100] = {
        "happy","sad","angry","computer","science","machine","learning","graph",
        "vector","database","index","fast","slow","memory","storage","data",
        "smart","intelligent","bright","dark","sun","moon","planet","earth",
        "galaxy","universe","star","sound","music","voice","language","word",
        "text","sentence","paragraph","book","paper","research","model","ai",
        "neural","network","brain","logic","reason","math","algebra","geometry",
        "physics","chemistry","biology","energy","power","speed","strength",
        "weak","small","large","tiny","huge","giant","micro","nano",
        "random","ordered","sorted","unsorted","tree","graph","node","edge",
        "connect","search","find","lookup","hash","table","array","list",
        "stack","queue","priority","weight","distance","similar","close","near",
        "far","distant","approximate","accurate","precise","true","false","maybe"
    };

    //------------------------------------
    // Build DB
    //------------------------------------
    VectorDB db;

    cout << "\nFetching embeddings for 100 words...\n\n";

    for(int i = 0; i < 100; i++){
        cout << "Requesting embedding for: " << words[i] << " ... ";

        Vector<float> emb = api.getEmbedding(words[i]);

        if(emb.length() == 0){
            cout << "[FAILED]\n";
            continue;
        }

        cout << "[OK]  Dimension = " << emb.length() << endl;

        Vector<float> norm = normalizeVector(emb);
        db.insert(norm);
    }

    cout << "\nInserted " << db.size()
         << " valid embedding vectors into DB.\n";

    //------------------------------------
    // Build Graph ANN
    //------------------------------------
    cout << "\nBuilding Graph ANN...\n";

    clock_t buildStart = clock();
    GraphANN ann(&db, 10);
    ann.buildGraph();
    clock_t buildEnd = clock();

    double buildTime =
        (double)(buildEnd - buildStart) / CLOCKS_PER_SEC * 1000.0;

    cout << "Graph Build Time: " << buildTime << " ms\n";

    SearchEngine brute(&db);

    int queries = 20;
    int topK = 10;

    double totalANN = 0;
    double totalBF = 0;
    int totalOverlap = 0;

    cout << "\nRunning " << queries << " random queries...\n";

    for(int q = 0; q < queries; q++){
        int idx = rand() % db.size();
        Vector<float> query = db.get(idx)->embedding;

        clock_t s1 = clock();
        Vector<Pair<int,float>> annRes = ann.search(query, topK);
        clock_t e1 = clock();

        clock_t s2 = clock();
        Vector<Pair<int,float>> bfRes = brute.search(query, topK);
        clock_t e2 = clock();

        double annT =
            (double)(e1 - s1) / CLOCKS_PER_SEC * 1000.0;

        double bfT =
            (double)(e2 - s2) / CLOCKS_PER_SEC * 1000.0;

        totalANN += annT;
        totalBF += bfT;

        int match = 0;

        for(int i = 0; i < annRes.length(); i++){
            for(int j = 0; j < bfRes.length(); j++){
                if(annRes[i].key == bfRes[j].key)
                    match++;
            }
        }

        totalOverlap += match;
    }

    cout << "\n================ RESULTS ================\n";
    cout << "Average ANN Time   : " << (totalANN / queries) << " ms\n";
    cout << "Average Brute Time : " << (totalBF / queries) << " ms\n";
    cout << "Speedup            : " << (totalBF / totalANN) << "x\n";
    cout << "Average Overlap    : "
         << (totalOverlap * 100.0 / (queries * topK))
         << "% of topK match\n";

    cout << "\n====================================\n";
    cout << " Benchmark Complete\n";
    cout << "====================================\n";

    return 0;
}
