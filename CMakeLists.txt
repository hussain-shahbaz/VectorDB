cmake_minimum_required(VERSION 3.10)
project(VectorDB CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Targets with no external dependencies — pure custom data structures / core
# vector engine, no libcurl or nlohmann/json needed.
set(CORE_TARGETS
    test_vector
    test_heap
    test_hashmap
    test_btree
    test_huffman
    test_pair
    test_list
    test_graph
    test_avl
    test_vectorDB
    test_graphann
    test_kdann
    test_ballann
    test_persistence
    benchmark_recall
)

foreach(target ${CORE_TARGETS})
    add_executable(${target} backend/tests/${target}.cpp)
endforeach()

# Embedding-dependent targets need libcurl (a system dependency) and
# nlohmann/json (vendored under backend/external, no install needed).
find_package(CURL)
if(CURL_FOUND)
    set(EMBEDDING_TARGETS test_embedding benchmark_cpp)
    foreach(target ${EMBEDDING_TARGETS})
        add_executable(${target} backend/tests/${target}.cpp)
        target_include_directories(${target} PRIVATE backend/external ${CURL_INCLUDE_DIRS})
        target_link_libraries(${target} PRIVATE ${CURL_LIBRARIES})
    endforeach()

    add_executable(vectordb backend/main.cpp)
    target_include_directories(vectordb PRIVATE backend/external ${CURL_INCLUDE_DIRS})
    target_link_libraries(vectordb PRIVATE ${CURL_LIBRARIES})

    add_executable(bulkload backend/tools/BulkLoad.cpp)
    target_include_directories(bulkload PRIVATE backend/external ${CURL_INCLUDE_DIRS})
    target_link_libraries(bulkload PRIVATE ${CURL_LIBRARIES})

    # The synonym server additionally needs the vendored Crow + standalone
    # Asio headers and pthreads (Crow's multithreaded() mode).
    find_package(Threads REQUIRED)
    add_executable(synonym_server backend/server/SynonymServer.cpp)
    target_include_directories(synonym_server PRIVATE backend/external backend/external/crow ${CURL_INCLUDE_DIRS})
    target_link_libraries(synonym_server PRIVATE ${CURL_LIBRARIES} Threads::Threads)
else()
    message(STATUS "libcurl not found — skipping vectordb, bulkload, synonym_server, test_embedding, benchmark_cpp (see README.md)")
endif()

# `ctest` runs everything in CORE_TARGETS that's actually a test (not the
# recall benchmark, which is meant to be read, not pass/failed). Deliberately
# excludes test_embedding/benchmark_cpp/vectordb: those need a live
# GEMINI_API_KEY and network access, which a CI runner won't have.
enable_testing()
list(REMOVE_ITEM CORE_TARGETS benchmark_recall)
foreach(target ${CORE_TARGETS})
    add_test(NAME ${target} COMMAND ${target})
endforeach()
