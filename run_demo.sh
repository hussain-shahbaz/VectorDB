#!/usr/bin/env bash
# Runs the full synonym demo: builds and starts the Crow backend (port
# 8080), then starts the React dev server (port 5173, proxying /api to the
# backend — see frontend/vite.config.ts). Ctrl-C stops both.
#
# Requires backend/data/vocabulary.vdb + vocabulary_words.txt to exist —
# build them first with the bulk loader (see README.md "Fill the DB"):
#   g++ -std=c++17 -Ibackend/external -o bulkload backend/tools/BulkLoad.cpp -lcurl
#   ./bulkload

set -e
cd "$(dirname "$0")"

if [ ! -f backend/data/vocabulary.vdb ] || [ ! -f backend/data/vocabulary_words.txt ]; then
    echo "backend/data/vocabulary.vdb (or its word list) is missing. Build it first:"
    echo "  g++ -std=c++17 -Ibackend/external -o bulkload backend/tools/BulkLoad.cpp -lcurl"
    echo "  ./bulkload"
    exit 1
fi

echo "Building synonym_server..."
g++ -std=c++17 -Ibackend/external -Ibackend/external/crow -pthread -o synonym_server backend/server/SynonymServer.cpp -lcurl

echo "Starting backend on :8080..."
./synonym_server &
BACKEND_PID=$!
trap 'kill "$BACKEND_PID" 2>/dev/null' EXIT

sleep 1

if [ ! -d frontend/node_modules ]; then
    echo "Installing frontend dependencies (first run only)..."
    (cd frontend && npm install)
fi

echo "Starting frontend dev server on :5173..."
cd frontend
npm run dev
