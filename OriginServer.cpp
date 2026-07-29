#include "OriginServer.h"

using namespace std;

bool OriginServer::hasFile(const string& file) const {
    return files.find(file) != files.end();
}

string OriginServer::getFile(const string& file) const {
    auto it = files.find(file);
    if (it == files.end()) return "";
    return it->second;
}
