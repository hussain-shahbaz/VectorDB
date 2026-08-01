# Build Plan

Live task tracker for VectorDB. Reflects actual verified state of the code as of 2026-08-01 — everything marked `[x]` was compiled and run (many against the live Gemini API), not just read. Update checkboxes as work lands; don't let this drift from reality.

Legend: `[x]` done and verified · `[~]` partially done / present but scoped-down · `[ ]` not started

## North star

Type a real word (e.g. `apple`) at a CLI, have it embedded through the live Gemini API, and get back a sensible top-k of semantically related words from a small seed corpus — proving the whole pipeline (`Embedding` → `VectorDB` → `SearchEngine`/`GraphANN`) works end-to-end against real data, not just synthetic test vectors. Achieved and since extended with persistence.

- [x] `backend/main.cpp` — seed corpus (25 words), live embedding, disk-backed `EmbeddingCache`, brute-force + GraphANN top-k query loop, interactive or one-shot
- [x] Verified against the real API: `./vectordb apple` → apple(1.0), banana(0.86), sad(0.83), orange(0.82), mango(0.82); `./vectordb motorcycle` (not in the seed corpus) → bicycle(0.87), truck(0.84) float to the top
- [x] `save`/`load`/`insert` subcommands, verified live: seeded 25 vectors → saved to disk → loaded fresh → queried by id → inserted a new word → reloaded and found it as the new top match

## North star 2 — full-stack synonym demo

Scale the CLI demo into something presentable: a real vocabulary, served over HTTP, searched from an actual React UI, "synonyms" being purely a product of vector similarity search — no synonym dataset lookup involved (see Phase 9 for why that was the explicit decision).

- [x] Curated a 3,000-word vocabulary from an open, no-auth source (Kaggle access wasn't available in this environment — no CLI/credentials configured — so this uses `first20hours/google-10000-english`'s frequency list instead, deduped and capped at 3,000). Kept as `backend/data/vocabulary_full.txt` for whenever quota allows finishing the rest (see below).
- [x] `backend/tools/BulkLoad.cpp` — embeds a word-list file through the live API, writes a persisted `.vdb` plus a word-id sidecar file (`VectorDB` doesn't persist metadata, so this is how the server maps ids back to words — see Phase 3)
- [x] Found and fixed a real bug while building this: `Embedding.h`'s curl call had **no timeout set at all** — a 10-word test run hung indefinitely on a stalled connection. Added `CURLOPT_CONNECTTIMEOUT`/`CURLOPT_TIMEOUT` plus a one-retry-on-failure in the bulk loader; verified fixed (the same 10-word test then completed in seconds) before launching the full run.
- [x] **Bulk-embedded 840 of the planned 3,000 words before hitting the Gemini free-tier daily quota** (`EmbedContentRequestsPerDayPerProjectPerModel-FreeTier`, 1,000 requests/day — confirmed via the API's own error response, not a guess). Stopped the run rather than let it burn through the remaining ~2,160 words all failing. Asked the user how to proceed (finish now with 840 / wait for quota reset / raise the quota) rather than assuming — they chose to finish now. `backend/data/vocabulary.txt` was trimmed from 3,000 to exactly the 840 words that succeeded (all safely disk-cached, no re-fetching needed); `vocabulary_full.txt` keeps the original 3,000 for later. Re-running `./bulkload` against the trimmed list completed in 0.13 seconds (100% cache hits) once quota-blocked, confirming no data was lost.
- [x] `backend/server/SynonymServer.cpp` — Crow (vendored, header-only) + standalone Asio (vendored, no Boost) HTTP API: `GET /api/synonyms?word=&k=&method=`, `GET /api/stats`. CORS enabled for local dev.
- [x] Found and fixed a real concurrency bug during design (not yet observed as a crash, but a genuine data race): Crow's `multithreaded()` mode dispatches concurrent requests to worker threads, but `Embedding`'s in-memory cache (`HashMap`+`List`) isn't synchronized. Guarded the `getEmbedding()` call in the server with a `std::mutex` — `SearchEngine`/`GraphANN` lookups don't need it, they're read-only over data that's never mutated after startup.
- [x] **Found and fixed a second real gap while testing against the real quota-exhausted key**: a freshly-started server's in-memory `Embedding` cache is empty, so querying a word that's already part of the bulk-loaded vocabulary still triggered a live API call — which failed, since quota was exhausted, even though that exact word had already been successfully embedded once during bulk load. Fixed by having the server check the disk-backed `EmbeddingCache` (shared with `main.cpp`/`bulkload`) before ever calling the live API — same mutex, since the disk cache isn't thread-safe either. Verified: the whole 840-word vocabulary is now instantly queryable with zero live calls; only genuinely new words need one.
- [x] `frontend/` — React + Vite + TypeScript, hand-styled (no UI framework), light/dark-mode aware. Search bar, example-word chips (updated to words that actually exist in the 840-word vocabulary, not the old 25-word CLI corpus), method toggle (GraphANN/brute-force), result cards with animated similarity bars, loading/error states.
- [x] **Verified end-to-end against the real 840-word dataset and the real (quota-exhausted) API key**, via headless-Chromium Playwright screenshots (no display in this environment, no `chromium-cli` available — used Playwright's own cached browser build instead): homepage shows "840 words indexed", clicking an example chip and typing a fresh query both return real ranked results with correct similarity percentages, zero console errors. Result quality is genuinely mixed at this vocabulary size — `strong` → strength/smart/ensure (good), `quick` → extra/focus/along/logic (noisy) — documented honestly rather than cherry-picking only the good examples.
- [x] `run_demo.sh` — builds and runs the Crow backend + React dev server together
- [x] `CMakeLists.txt` extended with `bulkload`/`synonym_server` targets (same unverified-locally caveat as the rest of the CMake file)

---

## Phase 0 — Foundational data structures (`backend/dataStructures/`)

- [x] `Vector<T>`, `Heap<T>`/`MinHeap`/`MaxHeap`, `Graph<T>`, `HashMap<K,V>`, `BTree<T>`, `Huffman`, `List<T>`, `Stack<T>`/`ListStack<T>`, `Queue<T>`/`ListQueue<T>`, `Pair<K,V>`, `AVL<T>` — all implemented and covered by a passing test file
- [x] `Vector::length()`/`lenght()` consolidated — the misspelled alias is gone, all call sites migrated
- [x] `HashMap<K,V>` generic-key fix — `hashCode`/`hash1`/`hash2` now go through `std::hash<K>` instead of a hardcoded `key*31` that only worked for `int`. `HashMap<string, V>` compiles and works correctly now (verified with a standalone test); no regressions on the existing `int`/`unsigned char` key usages (GraphANN's visited-set, Huffman's frequency table)
- [x] `List<T>` extended with `size()`, `insertAtFront()`, `search()` to match `test_list.cpp`'s expected API — while in there, fixed a real dangling-`tail`-pointer bug in `remove()` (removing the last element left `tail` pointing at freed memory; verified fixed under AddressSanitizer) and a `clear()` that wasn't actually deleting nodes (memory leak)
- [x] `Graph<T>` rewritten from position-only to label-keyed (`HashMap<T, Vector<T>>` internally) to match `test_graph.cpp`'s expected API (arbitrary node labels, e.g. `addNode(20)` then `getNeighbors(20)`). `addNode()` with no argument auto-labels sequentially, which is exactly what `GraphANN` needs — verified zero behavior change for `GraphANN` and the live demo after the rewrite
- [x] `BTree<T>` given a proper `clear()` (destroys and resets in place) — needed because `BTree` has no safe copy/assignment (compiler-generated shallow copy would double-free); `VectorDB::load()` relies on this to reset its index without leaking or crashing

## Phase 1 — Core vector engine (`backend/core/vector/`)

- [x] `VectorItem`, `VectorDB` (insert/get/at/size all correct — `insert()`'s off-by-one return-value bug is fixed), `Similarity` (cosine similarity now clamped to `[-1, 1]` — floating-point rounding was pushing self-similarity a hair past `1.0`, which was actually failing `test_vectorDB.cpp`'s Test 6 intermittently before the fix), `SearchEngine`, `GraphANN` — all implemented, tested, and exercised live
- [x] `KDANN` (KD-tree exact search, ranks by Euclidean distance) — implemented, and verified to exactly match a brute-force Euclidean reference on 300 random points across 15 query trials (`test_kdann.cpp`)
- [x] `BallANN` (Ball-tree exact search, same metric) — same verification approach, same result: exact match against brute force (`test_ballann.cpp`), plus a dedicated single-leaf-tree edge case
- [x] Soft delete (`VectorDB::remove(id)`) and update (`VectorDB::update(id, embedding)`) — wired through all four search classes (`SearchEngine`, `GraphANN`, `KDANN`, `BallANN`) so deleted vectors never appear in results. `GraphANN` still allocates a graph node per position (needed to keep auto-labels aligned with ids) but builds no outgoing edges from a deleted node; `KDANN`/`BallANN` simply exclude deleted ids when building the tree
- [x] Metadata filtering — `VectorItem` carries a `HashMap<string,string>` (now that `HashMap` supports string keys) plus a `MetadataFilter{key,value}` struct; every `search()` takes an optional `const MetadataFilter* filter = nullptr`. Verified: filtering by `category=fruit` correctly excludes a near-duplicate vector tagged `category=vegetable`

## Phase 2 — Embedding / Gemini API integration (`backend/core/Embedding/`)

- [x] `Embedding.h` compiles and runs on Linux (removed the pointless `<windows.h>` include, dropped the debug `cout` that dumped the full raw API response on every call and the stray `answer.json` disk write, added real `try`/`catch` around JSON parsing with errors to `cerr` instead of silently returning empty)
- [x] API key handling — loaded via `backend/common/EnvUtil.h` from env var or `.env`, never hardcoded (see Security note below)
- [x] Vendored `nlohmann/json.hpp` at `backend/external/nlohmann/json.hpp` — no system package dependency
- [x] In-memory LRU response cache inside `Embedding` itself (`HashMap` for O(1) lookup + `List` tracking usage order for eviction), separate from and complementary to `main.cpp`'s disk-backed `EmbeddingCache`. Verified live: a repeated request for the same word returned in 0ms instead of hitting the network, cache size stayed at 1 instead of growing

### Security note (resolved 2026-08-01)
A live Gemini API key was hardcoded in two test files, committed since `d077d54`. Fixed: moved to `.env`/env var loading, scrubbed from local git history (`filter-branch` + `reflog expire` + `gc --prune=now`), force-pushed the rewritten history to `origin/master`. **Key was not rotated** — user opted to keep using the same key; if it was ever public on GitHub before the scrub, treat it as potentially harvested by scanners.

## Phase 3 — Persistent storage layer

- [x] `backend/core/storage/BtreeIndex.h` — id → position index, `BTree<Pair<int,int>>` under the hood (`Pair`'s comparison operators only look at `.key`, so a set-only `BTree<T>` works as a map for free)
- [x] `backend/core/storage/HuffmanCompression.h` — `Vector<float>` ⟷ raw bytes conversion, drives `Huffman::encode`/`decode`
- [x] `Huffman` extended with `serializeTree()`/`loadTree()` — necessary because `decode()` only ever walks the tree built by the *same* `encode()` call, which doesn't survive a process restart. Verified with a cross-object round trip (encode in one `Huffman`, decode in a completely different one after loading the serialized tree) and the single-symbol edge case (root with only a left child, which an earlier version of the serializer would have desynced on)
- [x] `VectorDB::save(path)` / `VectorDB::load(path)` — binary format: header (magic/version/count/dim) + id table + one shared Huffman-compressed block for all vectors. `save()` compacts away soft-deleted items
- [x] Round-trip test suite (`test_persistence.cpp`, 6 tests, all passing): bit-exact embedding survival, identical `SearchEngine` results before/after reload, soft-deleted items dropped on save, empty-DB round trip, graceful failure on a missing/corrupt file, `nextId` doesn't collide after reload
- [~] **Honestly scoped, not a full implementation of the spec's design**: vectors share *one* Huffman tree (better compression, and matches how `Huffman.h` is actually meant to be used — see Phase 0), so the on-disk "index" is a flat id table rather than a serialized tree with true random-access byte offsets into the compressed data. Loading is eager (whole file decompressed at once), not lazy. Per-item metadata is **not** persisted. All of this is documented in `project_documentation.tex` Appendix D and in code comments — see that appendix if this needs to be revisited.

## Phase 4 — Build system & tooling

- [x] `run_tests.sh` — compiles and runs every `backend/tests/*.cpp` with plain `g++`, reports pass/fail, skips the embedding test gracefully if no API key is available. **Verified working**: 15/15 pass, correct exit code. **Found and fixed a real bug in the script itself**: `test_embedding.cpp` returns non-zero and prints an "Error: ..." line (not "FAIL:") on failure, but the script only grepped for `^FAIL` — so a genuinely failing live API call (discovered when the free-tier quota got exhausted mid-session, see North star 2) was reported as a false `[PASS]`. Fixed to also check the exit code for that specific test.
- [~] `CMakeLists.txt` — written (core targets + libcurl-gated embedding targets + `ctest` wiring), but **unverified on this machine** — no `cmake` binary available here and no passwordless `sudo` to install it. The GitHub Actions workflow below will be the first real signal on whether it's actually correct; treat it as unverified until CI runs green.
- [x] C++ standard pinned to C++17 (`-std=c++17` everywhere; also `CMAKE_CXX_STANDARD 17` in the CMake file)
- [x] CI — `.github/workflows/ci.yml` runs `run_tests.sh` (the verified path) plus a `cmake`+`ctest` build (the unverified path, so CI is what actually validates it) on every push/PR. `GEMINI_API_KEY` is read from an optional repo secret; without it, `run_tests.sh` skips the live test and still passes.

## Phase 5 — CLI / entry point

- [x] `backend/main.cpp` — `save`/`load`/`insert` subcommands added on top of the existing seed-and-query default, all exercised live against the real API and real files: seed→save, load→query-by-id, insert→save, reload→find-the-new-item. Note `load` prints results by numeric id, not by word — metadata (which would carry the source text) isn't persisted, see Phase 3.

## Phase 6 — Testing & QA

All of the following compile clean and pass with zero unexpected `FAIL` lines (verified via `run_tests.sh` and individually):

- [x] `test_vector`, `test_heap`, `test_hashmap`, `test_btree`, `test_huffman` (now including 2 new tree-serialization tests), `test_pair`, `test_vectorDB` (now including 3 new tests: soft-delete, update, metadata filtering), `test_graphann`
- [x] `test_list` — fixed (was broken: bad include path + missing `size()`/`insertAtFront()`/`search()`)
- [x] `test_graph` — fixed (was broken: bad include path + `Graph` rewritten to be label-keyed)
- [x] `test_avl` — added from scratch (didn't exist despite `AVL.h` being fully implemented); verifies real balance (height 9 for 1,000 sorted inserts, vs. a degenerate chain of 1,000 for a plain BST)
- [x] `test_kdann`, `test_ballann` — added from scratch, verify exact-match correctness against independent brute-force references
- [x] `test_persistence` — added from scratch, 6-test round-trip suite (see Phase 3)
- [x] `test_embedding` — runs live against the real API; also verifies the new in-memory cache
- [x] `benchmark_cpp` — runs live against the real API (95 words, all succeed now). **Found and fixed a real bug**: the word-list array was declared `string words[100]` but the literal only had 95 entries, so the last 5 were empty strings sent to the API as invalid requests, all failing. Also added disk caching (shares `.embedding_cache` with `main.cpp`) so re-runs don't re-spend quota.
- [x] `benchmark_recall` — added from scratch: measures `GraphANN` Recall@10 and speedup vs. `SearchEngine` on synthetic data, n = 1,000 to 8,000. **Real finding**: recall declined from ~0.80 to ~0.55 and speedup topped out around 4× — well short of the report's aspirational >90% recall / >10× speedup — because `GraphANN`'s search budget is hardcoded to `k*2` regardless of database size. This is now the clearest concrete next step for anyone continuing `GraphANN` (see Future work below).

## Phase 7 — Cleanup

- [x] `backend/tests/toolkit.cpp` and `new.cpp` deleted — confirmed with the repo owner first (they were unrelated Windows-only console utilities, not part of VectorDB)
- [x] `backend/docs/` PDFs deduped — `VectorDB (3).pdf` was a stale export missing the KD-Tree/Ball-Tree sections (confirmed via `pdftotext` diff); kept `VectorDB (4).pdf`, renamed to `project_documentation.pdf`
- [x] `length()`/`lenght()` duplicate naming resolved (Phase 0)

## Phase 8 — Documentation

- [x] `README.md`, `build-plan.md` (this file), `CLAUDE.md` (gitignored) — all current
- [x] `project_documentation.tex` synced: Appendix C's project structure updated to match reality, new Appendix D added stating plainly which components are implemented, how persistence is honestly scoped, and — importantly — that the Performance Analysis tables earlier in the report were originally illustrative targets, not measurements, with real measured numbers now available via `benchmark_cpp.cpp`/`benchmark_recall.cpp`
- [ ] **`project_documentation.pdf` is now stale relative to the `.tex` source** — no LaTeX toolchain (`pdflatex` or equivalent) is available in this environment to regenerate it. Whoever has LaTeX installed needs to rebuild it from the `.tex` source before submission/sharing.

## Phase 9 — Synonym demo (data, server, frontend)

Decisions made explicitly with the user before building (asked rather than assumed, per instruction):

- **Kaggle access**: no Kaggle CLI or credentials were configured in this environment, so direct Kaggle download wasn't possible. User chose: skip Kaggle, use a comparable open dataset instead. Used `first20hours/google-10000-english`'s frequency list (plain text, no auth), deduped and capped at 3,000 words.
- **Scale**: user chose "medium" (~2,000–5,000 words) over small (~200–500) or large (10,000+). Landed on exactly 3,000.
- **What "synonyms" means**: user chose vector similarity search (nearest neighbors by embedding) over dataset ground-truth lookup or a side-by-side of both. This is consistent with the whole project's premise — see the README's Known limitations for the explicit caveat that this surfaces related/similar words, not strictly dictionary synonyms.
- **Stray files cleanup during the earlier build-plan pass** (`toolkit.cpp`/`new.cpp` deletion) was confirmed with the user before deleting, not assumed — noted here since it's the same "ask, don't assume" pattern the user asked to be followed for this phase too.
- **Quota exhaustion mid-run**: hit the Gemini free-tier daily quota (1,000 requests/day) at 840/3,000 words. Asked the user rather than guessing whether to (a) finish now with 840, (b) wait ~24h for quota reset and resume, or (c) raise the quota via billing. They chose (a).

- [x] `backend/data/vocabulary_full.txt` — the original curated 3,000-word source list, committed for future reference/expansion
- [x] `backend/data/vocabulary.txt` — trimmed to the 840 words that actually got embedded before the quota hit (same source, same order, just shorter) — this is the list that matches what's actually loaded in `vocabulary.vdb`
- [x] `backend/data/vocabulary.vdb` + `vocabulary_words.txt` — generated by the bulk loader, 840 words, committed so the repo is runnable out of the box (`synonym_server` + `frontend`) without anyone needing their own API key/quota just to see the demo. A fresh `GEMINI_API_KEY` is still needed for typing genuinely *new* query words not already in the 840-word vocabulary (see the disk-cache fix above — anything in the vocabulary is instantly queryable with zero live calls).
- [x] Data pipeline, server, and frontend all built and verified end-to-end against the real 840-word dataset and the real (quota-exhausted) key — see North star 2 above
- [ ] Grow the vocabulary from 840 back toward the full 3,000 once quota allows: `./bulkload backend/data/vocabulary_full.txt backend/data/vocabulary.vdb backend/data/vocabulary_words.txt` — the 840 already-cached words cost nothing to re-embed (cache hits), only the remaining ~2,160 spend fresh quota. At 1,000 requests/day that's about 3 more days if starting fresh, less if quota resets sooner than 24h from the original run.
- [ ] Decide whether to also commit `frontend/dist` (a built static bundle) or leave the frontend as dev-server-only — currently only wired for `npm run dev`, no static-serving/deployment story

---

## Future work (discovered during this pass, not yet started)

- [ ] `GraphANN`'s search budget is a hardcoded `k*2` — make it a tunable `efSearch` parameter (per the report's own Section 5.3.4) so recall doesn't silently degrade as the database grows. This is the single highest-value follow-up; `benchmark_recall.cpp` is ready to re-measure once it's tunable.
- [ ] Persist per-item metadata in `VectorDB::save`/`load` — would let `SynonymServer` read words directly from the loaded DB instead of needing a separate sidecar file
- [ ] Give vectors independent, randomly-seekable compressed blocks (per-vector Huffman trees, or a shared-dictionary scheme) if lazy/partial loading ever becomes necessary at larger scale
- [ ] `VectorDB` (and `BtreeIndex`/`BTree`) still aren't safely copyable — copying would shallow-copy internal pointers and double-free on destruction. Not currently exercised anywhere in the codebase, but worth knowing before anyone passes a `VectorDB` by value
- [ ] Regenerate `project_documentation.pdf` from the updated `.tex` once a LaTeX toolchain is available
- [ ] Bulk-embedding is strictly sequential (one live API call at a time) — batching or bounded concurrency would meaningfully cut the load time for a few thousand words, though it wouldn't help with the free-tier daily *quota* ceiling itself, only the time spent within it
- [ ] `BulkLoad.cpp` doesn't detect quota exhaustion specifically (`RESOURCE_EXHAUSTED`/429) vs. a transient timeout — right now both just get one retry and a skip. Once quota is hit, every remaining word fails the same way, which is wasted time (even if fast) iterating a doomed list. Worth detecting the specific error and stopping the run early with a clear message, rather than relying on a human to notice and kill it (which is what happened this session)
- [ ] Grow the vocabulary back toward 3,000 words (see Phase 9) once daily quota allows
- [ ] No production deployment story for `frontend`/`synonym_server` (static build, process manager, reverse proxy, etc.) — currently dev-server-only, which is fine for a course demo but not for anything beyond that
