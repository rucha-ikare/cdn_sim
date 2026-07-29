#include "Server.h"

using namespace std;

Server::Server(string n, Coordinates c, bool on)
    : name(n), coord(c), online(on) {}

bool Server::isOnline() const {
    return online;
}

void Server::setOnline(bool on) {
    online = on;
}

bool Server::cacheHas(const string& file) const {
    return cache.find(file) != cache.end();
}

string Server::getFromCache(const string& file) const {
    auto it = cache.find(file);
    if (it == cache.end()) return "";
    return it->second;
}

void Server::putInCache(const string& file, const string& content) {
    cache[file] = content;
}
