#!/usr/bin/env bash
# Compiles and runs every backend/tests/*.cpp test with plain g++ (no CMake
# required) and reports a pass/fail summary. This is the CI-friendly path —
# see build-plan.md Phase 4 for why there's no CMake-only story yet.
#
# Usage: ./run_tests.sh
# Exit code: 0 if everything compiled and no test printed a "FAIL:" line.

set -u
cd "$(dirname "$0")"

BUILD_DIR="$(mktemp -d)"
trap 'rm -rf "$BUILD_DIR"' EXIT

CORE_TESTS=(
    test_vector test_heap test_hashmap test_btree test_huffman test_pair
    test_list test_graph test_avl test_vectorDB test_graphann test_kdann
    test_ballann test_persistence
)

PASS_COUNT=0
FAIL_COUNT=0
FAILED_NAMES=()

run_one() {
    local name="$1"
    local src="backend/tests/${name}.cpp"
    local bin="$BUILD_DIR/${name}"

    if ! g++ -std=c++17 -O0 -o "$bin" "$src" 2> "$BUILD_DIR/${name}.compile.log"; then
        echo "[COMPILE FAIL] $name"
        sed 's/^/    /' "$BUILD_DIR/${name}.compile.log" | head -20
        FAIL_COUNT=$((FAIL_COUNT + 1))
        FAILED_NAMES+=("$name (compile error)")
        return
    fi

    local out
    out="$("$bin" 2>&1)"
    local unexpected_fails
    unexpected_fails="$(printf '%s\n' "$out" | grep -c '^FAIL')"

    if [ "$unexpected_fails" -eq 0 ]; then
        echo "[PASS] $name"
        PASS_COUNT=$((PASS_COUNT + 1))
    else
        echo "[FAIL] $name ($unexpected_fails failing assertion(s))"
        printf '%s\n' "$out" | grep '^FAIL' | sed 's/^/    /'
        FAIL_COUNT=$((FAIL_COUNT + 1))
        FAILED_NAMES+=("$name")
    fi
}

echo "=== Core test suite (no external dependencies) ==="
for t in "${CORE_TESTS[@]}"; do
    run_one "$t"
done

echo
echo "=== Embedding tests (need GEMINI_API_KEY + libcurl + network) ==="
if [ -z "${GEMINI_API_KEY:-}" ] && [ ! -f .env ]; then
    echo "[SKIP] test_embedding — no GEMINI_API_KEY env var or .env file found"
else
    if g++ -std=c++17 -Ibackend/external -o "$BUILD_DIR/test_embedding" backend/tests/test_embedding.cpp -lcurl 2> "$BUILD_DIR/test_embedding.compile.log"; then
        # Unlike the CORE_TESTS (which always exit 0 and rely purely on the
        # FAIL:/PASS: print convention), test_embedding is a smoke test that
        # returns non-zero on failure without necessarily printing "FAIL:" —
        # a grep-only check here previously reported a false PASS when the
        # live API call failed (e.g. quota exhausted). Check both.
        (cd backend/tests && "$BUILD_DIR/test_embedding" > "$BUILD_DIR/test_embedding.out" 2>&1)
        embedding_exit=$?
        if [ "$embedding_exit" -ne 0 ] || grep -q '^FAIL' "$BUILD_DIR/test_embedding.out"; then
            echo "[FAIL] test_embedding (exit code $embedding_exit)"
            tail -5 "$BUILD_DIR/test_embedding.out" | sed 's/^/    /'
            FAIL_COUNT=$((FAIL_COUNT + 1))
            FAILED_NAMES+=("test_embedding")
        else
            echo "[PASS] test_embedding (live API call)"
            PASS_COUNT=$((PASS_COUNT + 1))
        fi
    else
        echo "[COMPILE FAIL] test_embedding"
        sed 's/^/    /' "$BUILD_DIR/test_embedding.compile.log" | head -20
        FAIL_COUNT=$((FAIL_COUNT + 1))
        FAILED_NAMES+=("test_embedding (compile error)")
    fi
fi

echo
echo "======================================"
echo "Results: $PASS_COUNT passed, $FAIL_COUNT failed"
if [ "$FAIL_COUNT" -gt 0 ]; then
    echo "Failed: ${FAILED_NAMES[*]}"
fi
echo "======================================"

[ "$FAIL_COUNT" -eq 0 ]
