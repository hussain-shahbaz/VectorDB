#ifndef ENV_UTIL_H
#define ENV_UTIL_H

#include <cstdlib>
#include <fstream>
#include <string>

// Reads `key` from the process environment first; falls back to a simple
// KEY=VALUE line in a .env file so secrets never need to be hardcoded in
// source. `envPath` is resolved relative to the process's cwd at runtime,
// not the including source file, so callers in different directories need
// to pass the right relative path (tests/ run two levels below the repo
// root; main.cpp is meant to be run from the repo root). Returns "" if not
// found anywhere.
inline std::string loadEnvVar(const std::string& key, const std::string& envPath = "../../.env") {
    if (const char* fromEnv = std::getenv(key.c_str())) {
        return std::string(fromEnv);
    }

    std::ifstream file(envPath);
    std::string line;
    while (std::getline(file, line)) {
        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;
        if (line.substr(0, eq) == key) return line.substr(eq + 1);
    }
    return "";
}

#endif
