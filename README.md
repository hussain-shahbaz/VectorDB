# VectorDB

A vector database built from scratch in C++ — custom data structures (no STL containers in the core layers), cosine/Euclidean similarity search, four different nearest-neighbor strategies, B-tree + Huffman backed persistence, an HTTP API (Crow), and a React frontend, all wired up end-to-end against the live Gemini embedding API. Built as a DSA course project (CSC-202, UET Lahore) to demonstrate that the fundamentals — dynamic arrays, heaps, graphs, hash maps, B-trees, Huffman coding — are what modern vector search engines are actually made of.

The flagship demo is a **semantic synonym finder**: a real vocabulary embedded via Gemini, served over HTTP, searched from a real React UI — "synonyms" here means nearest neighbors by embedding similarity, not a lookup table. See [Synonym demo](#synonym-demo-full-stack) below, or [Try it](#try-it) for the plain CLI. [build-plan.md](build-plan.md) has the full, honestly-tracked task breakdown, including what's still scoped down or aspirational.

## Quick start

The fastest way to see it running — the 840-word vocabulary is already committed (`backend/data/vocabulary.vdb`), so no bulk-loading step is needed:

```bash
# 1. Add a Gemini API key (only needed for querying words outside the
#    pre-loaded vocabulary — the demo works without one otherwise, since
#    the server falls back to a 400/502 with a clear error for those)
echo "GEMINI_API_KEY=your-key-here" > .env

# 2. Install frontend dependencies (first time only)
cd frontend && npm install && cd ..

# 3. Build and run the backend + frontend together
./run_demo.sh
```

Then open **http://localhost:5173**. Requires a C++17 compiler, libcurl dev headers, and Node.js — see [Building](#building) if any of those need installing. For the plain C++ CLI instead of the full web app, skip to [Try it](#try-it); for what each piece does, see [Synonym demo (full stack)](#synonym-demo-full-stack) below.

## Features

**Implemented:**
- Hand-rolled data structures: dynamic array (`Vector`), min/max binary heap, graph (`Graph`, label-keyed adjacency list), hash map (`HashMap`, open addressing + double hashing, generic key hashing via `std::hash<K>`), B-tree, Huffman tree (with tree serialization for cross-process decoding), singly linked list, array- and list-backed stack/queue, AVL tree, key-value `Pair`
- `VectorDB` — vector store with insert, id lookup (via an internal `BtreeIndex`, O(log n)), soft delete, update, and disk persistence
- Four search strategies, all correctness-tested:
  - `SearchEngine` — exact brute-force k-NN (cosine similarity)
  - `GraphANN` — approximate k-NN via a navigable graph index (HNSW-inspired)
  - `KDANN` — exact k-NN via a KD-tree (Euclidean distance)
  - `BallANN` — exact k-NN via a Ball-tree (Euclidean distance)
- Metadata filtering — tag items with key/value metadata, restrict search to items matching a filter
- `Embedding` — Gemini API client (128-dim), with an in-memory LRU response cache
- Persistence — `VectorDB::save()`/`load()`, B-tree index + Huffman-compressed vectors on disk
- `backend/main.cpp` — a CLI with `save`/`load`/`insert` subcommands plus the original seed-and-query demo, all exercised live
- `backend/tools/BulkLoad.cpp` — embeds an entire word-list file through the live API and persists the result, with retry-on-timeout and a disk cache so re-runs don't re-spend quota
- `backend/server/SynonymServer.cpp` — an HTTP API (Crow, header-only, vendored) serving nearest-neighbor "synonym" lookups over the bulk-loaded vocabulary
- `frontend/` — a React + Vite UI for the synonym server: type a word, see ranked results with similarity bars, toggle exact vs. approximate search
- A build system: `CMakeLists.txt` + `run_tests.sh` (the verified path — see [Building](#building)) + GitHub Actions CI

**Scoped down / not implemented** (see `build-plan.md` "Future work" for the honest detail on each):
- Persistence doesn't persist per-item metadata, and loads eagerly (whole file decompressed at once) rather than lazily
- `GraphANN`'s search budget is a fixed `k*2`, not scaled to database size — recall measurably degrades as the database grows (see the Known limitations section below)
- No HTTP API, multi-threading, or distributed/sharding architecture

## Architecture

```
┌─────────────────────────────────────────┐
│         User Interface Layer             │   ← CLI (backend/main.cpp): seed/query/save/load/insert
│    (CLI / API for queries & inserts)     │
└────────────────┬────────────────────────┘
                  │
┌─────────────────▼────────────────────────┐
│         Search Engine Layer               │
│  (Brute Force / GraphANN / KDANN /        │   ← implemented, all 4 strategies tested
│   BallANN)                                │
└────────────────┬────────────────────────┘
                  │
┌─────────────────▼────────────────────────┐
│      Vector Storage Layer (VectorDB)      │   ← implemented (id lookup via BtreeIndex)
└─────┬──────────────────────────┬──────────┘
      │                          │
┌─────▼──────────┐     ┌─────────▼─────────┐
│  Embedding API  │     │  Disk Storage      │
│  (Gemini API)   │     │  (B-Tree + Huff)   │   ← implemented, see Known limitations
└─────────────────┘     └────────────────────┘
```

The full design writeup lives in [`backend/docs/project_documentation.tex`](backend/docs/project_documentation.tex) (compiles to a PDF report). Its Appendix D honestly tracks implementation status against the original design, including which of its own Performance Analysis numbers were illustrative targets vs. what's actually been measured.

## Try it

With a `GEMINI_API_KEY` in a `.env` file at the repo root:

```bash
g++ -std=c++17 -Ibackend/external -o vectordb backend/main.cpp -lcurl
./vectordb apple
```

```
=== Query: "apple" (top 5) ===

Brute-force (SearchEngine, exact):
  1. apple  (similarity: 1.0000)
  2. banana  (similarity: 0.8586)
  3. sad  (similarity: 0.8298)
  4. orange  (similarity: 0.8246)
  5. mango  (similarity: 0.8242)

GraphANN (approximate):
  1. apple  (similarity: 1.0000)
  2. banana  (similarity: 0.8586)
  3. sad  (similarity: 0.8298)
  4. orange  (similarity: 0.8246)
  5. mango  (similarity: 0.8242)
```

Run `./vectordb` with no arguments for an interactive prompt, or `./vectordb <word> <k>` for a one-shot query. First run embeds the ~25-word seed corpus live; after that, everything is served from a local cache (`.embedding_cache`, gitignored) except genuinely new query words.

**Persistence**, from the same binary:

```bash
./vectordb save mydb.vdb              # seed the demo corpus, persist it
./vectordb load mydb.vdb tiger 3      # load it back, query by id (metadata isn't persisted, so no word labels)
./vectordb insert mydb.vdb motorcycle # embed + insert a new word, save back
```

## Synonym demo (full stack)

The bigger demo: a real vocabulary — currently **840 words** — sourced from an open [word-frequency list](https://github.com/first20hours/google-10000-english) (no Kaggle auth required), embedded live and served through an actual HTTP API to a React frontend. `backend/data/vocabulary.vdb`/`vocabulary_words.txt` are committed, so this works out of the box with no setup beyond building — no API key needed just to browse the pre-loaded vocabulary (only for typing a genuinely new query word).

**Why 840 and not the full 3,000 words curated** (`backend/data/vocabulary_full.txt`): the bulk embed hit the Gemini free-tier daily quota (1,000 requests/day) partway through. 840 words is still a solid, working demo — see `build-plan.md`'s "North star 2" for the full story, and the note below on growing it back to 3,000 once quota allows.

**Already built?** Skip straight to [Run everything](#run-everything-with-real-data) below — `vocabulary.vdb` is committed. Only rebuild it if you want a different/larger vocabulary:

```bash
g++ -std=c++17 -Ibackend/external -o bulkload backend/tools/BulkLoad.cpp -lcurl
./bulkload   # writes backend/data/vocabulary.vdb + vocabulary_words.txt from vocabulary.txt

# To grow toward the full 3,000-word list instead (needs GEMINI_API_KEY,
# spends live quota only on words not already cached):
./bulkload backend/data/vocabulary_full.txt backend/data/vocabulary.vdb backend/data/vocabulary_words.txt
```

**Run everything with real data:**

```bash
./run_demo.sh
```

This builds and starts the Crow backend on `:8080`, then the React dev server on `:5173` (proxying `/api/*` to the backend — see `frontend/vite.config.ts`). Open `http://localhost:5173`.

Or run the pieces yourself:

```bash
g++ -std=c++17 -Ibackend/external -Ibackend/external/crow -pthread -o synonym_server backend/server/SynonymServer.cpp -lcurl
./synonym_server              # HTTP API on :8080

cd frontend && npm install && npm run dev   # UI on :5173
```

**API**, if you just want to hit it directly:

```bash
curl "http://localhost:8080/api/synonyms?word=happy&k=10&method=graph"
curl "http://localhost:8080/api/stats"
```

`method` is `graph` (GraphANN, approximate) or `brute` (SearchEngine, exact) — the UI lets you toggle between them. Note the server only persists vector data + a word-id sidecar file (see `backend/tools/BulkLoad.cpp`'s comment on why); the server checks the disk-backed embedding cache before ever hitting the live API, so **any word already in `vocabulary.txt` is queryable with zero API calls** — a fresh `GEMINI_API_KEY` is only needed for typing a genuinely new word.

## Project structure

```
VectorDB/
├── backend/
│   ├── core/
│   │   ├── vector/          VectorItem, VectorDB, Similarity, SearchEngine, GraphANN, KDANN, BallANN
│   │   ├── Embedding/        Gemini API client (with in-memory LRU cache)
│   │   └── storage/           BtreeIndex, HuffmanCompression — persistence support
│   ├── dataStructures/      Vector, Heap, Graph, HashMap, BTree, Huffman, List, Queue, Stack, Pair, AVL
│   ├── common/               EnvUtil.h — shared .env/env-var loader
│   ├── external/              vendored nlohmann/json.hpp, crow/crow_all.h, asio (no system packages needed)
│   ├── tools/                  BulkLoad.cpp — embeds a word list, persists a .vdb + word sidecar file
│   ├── server/                 SynonymServer.cpp — Crow HTTP API over a persisted VectorDB
│   ├── data/                   vocabulary.txt (840 words, matches the committed .vdb), vocabulary_full.txt (the original 3,000), vocabulary.vdb/_words.txt (committed, generated)
│   ├── tests/                 test_*.cpp per module, benchmark_cpp.cpp, benchmark_recall.cpp
│   ├── docs/                   LaTeX project report + reference material
│   ├── EmbeddingCache.h        disk-backed word→embedding cache used by main.cpp
│   └── main.cpp                 CLI — seed/query/save/load/insert, see Try it above
├── frontend/                  React + Vite UI for the synonym server
├── CMakeLists.txt             unverified locally (no cmake on the dev machine) — see Building
├── run_tests.sh                verified test runner, no build system required
├── run_demo.sh                  builds + runs the Crow backend and React frontend together
├── .github/workflows/ci.yml    runs run_tests.sh + a CMake build on every push
├── README.md
└── build-plan.md              task tracker / roadmap
```

## Building

Two ways to build, in order of how well-verified they are on the machine this was developed on:

**1. Plain `g++` / `run_tests.sh` (verified working):**

```bash
./run_tests.sh   # compiles and runs every test in backend/tests/, reports pass/fail
```

Or compile things individually:

```bash
g++ -std=c++17 -O2 -o vectordb_test backend/tests/test_vectorDB.cpp
./vectordb_test
```

Include paths in every source file are relative to the file itself, so it doesn't matter what directory you invoke `g++` from — the file layout above just needs to stay intact.

**2. CMake (written, but not verified on this machine — no `cmake` binary was available to test it here; CI is what actually validates it):**

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

The embedding-related builds (`test_embedding.cpp`, `benchmark_cpp.cpp`, `main.cpp`, `bulkload`, `synonym_server`) additionally need:
- [libcurl](https://curl.se/libcurl/) dev headers (`pkg-config libcurl`) — a system dependency, install via your package manager (e.g. `libcurl4-openssl-dev` on Debian/Ubuntu, `curl` on Arch)
- `nlohmann/json` — already vendored at `backend/external/nlohmann/json.hpp`, just add `-Ibackend/external` when compiling
- A `GEMINI_API_KEY` — put it in a `.env` file at the repo root (`GEMINI_API_KEY=...`) or export it as an environment variable. Never hardcode it in source — `backend/common/EnvUtil.h` handles loading it.

`synonym_server` additionally needs Crow + standalone Asio, both vendored at `backend/external/crow/crow_all.h` and `backend/external/asio.hpp` (`-Ibackend/external/crow -Ibackend/external`) — no Boost required. And `-pthread`, for Crow's multithreaded mode.

`frontend/` needs Node.js (developed against v22) — `npm install && npm run dev`.

## Running tests

`./run_tests.sh` is the easiest way — compiles and runs everything, prints a pass/fail summary, skips the live-API test gracefully if no key is configured. Individually:

```bash
g++ -std=c++17 -o /tmp/t backend/tests/test_vector.cpp      && /tmp/t
g++ -std=c++17 -o /tmp/t backend/tests/test_list.cpp        && /tmp/t
g++ -std=c++17 -o /tmp/t backend/tests/test_graph.cpp       && /tmp/t
g++ -std=c++17 -o /tmp/t backend/tests/test_avl.cpp         && /tmp/t
g++ -std=c++17 -o /tmp/t backend/tests/test_kdann.cpp       && /tmp/t
g++ -std=c++17 -o /tmp/t backend/tests/test_ballann.cpp     && /tmp/t
g++ -std=c++17 -o /tmp/t backend/tests/test_persistence.cpp && /tmp/t
# ...and similarly for test_heap, test_hashmap, test_btree, test_huffman,
# test_pair, test_vectorDB, test_graphann.
```

There's no assertion framework — each test prints expected-vs-actual and a `PASS`/`FAIL` line per case (lines starting with `FAIL` are real failures; `"FAILED (correct)"` mid-sentence is a test deliberately checking failure behavior).

## Known limitations

- **The demo vocabulary is 840 words, not the 3,000 originally planned.** The bulk embed hit Gemini's free-tier daily quota (1,000 requests/day) partway through. `backend/data/vocabulary_full.txt` has the full curated 3,000; re-run `bulkload` against it once quota allows to grow the live vocabulary (the 840 already-embedded words are cache hits, so only the remainder spends fresh quota).
- **Recall degrades with scale.** `benchmark_recall.cpp` measures `GraphANN` against exact `SearchEngine` on synthetic data from n=1,000 to 8,000: recall declined from ~0.80 to ~0.55 and speedup topped out around 4× — short of the project report's aspirational >90% recall / >10× speedup — because the search budget is hardcoded to `k*2` rather than scaling with database size. Tracked as the top item in `build-plan.md`'s Future Work.
- Persistence doesn't save per-item metadata, and reloads the whole file eagerly rather than supporting random access into individual compressed vectors. That's why `SynonymServer` needs a separate word-list sidecar file rather than reading words back out of the loaded `VectorDB`.
- "Synonyms" from the demo are nearest neighbors by cosine similarity over Gemini embeddings, not ground-truth synonym pairs — related words, antonyms, and same-category words can all score highly. At 840 words some queries are notably better than others (`strong` → strength/smart is solid; `quick` → extra/focus/along is noisy) — that's the honest, observed behavior of embedding similarity search over a general word-frequency vocabulary, not a bug to hide.
- `SynonymServer` serializes calls to `Embedding::getEmbedding()` and the disk-backed `EmbeddingCache` behind a mutex, since Crow's multithreaded mode can dispatch concurrent requests but neither cache is internally synchronized. `SearchEngine`/`GraphANN` lookups themselves are read-only and don't need this.
- Bulk-embedding is sequential (one live API call per word, ~2 seconds each including occasional timeout-and-retry) — there's no concurrent/batched embedding yet, and `bulkload` doesn't specifically detect quota exhaustion vs. a transient timeout (it just retries once and moves on either way, which is harmless but slow once quota's actually gone).
- The CMake build is unverified locally (no `cmake` available in the dev environment) — CI is the first real check on it.
- `project_documentation.pdf` is stale relative to the `.tex` source (no LaTeX toolchain available here to regenerate it after the latest edits).

## Authors

Hussain Shahbaz (2024-CS-04) and Ali Hassan Siddiqui (2024-CS-05) — CSC-202 Data Structures & Algorithms, Department of Computer Science, University of Engineering and Technology, Lahore. Supervised by Mr. Nazeef Ul Haq.
