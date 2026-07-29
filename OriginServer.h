#pragma once
#include <string>
#include <unordered_map>

using namespace std;

class OriginServer {
public:
    unordered_map<string, string> files;

    bool hasFile(const string& file) const;
    string getFile(const string& file) const;
};
