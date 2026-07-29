#pragma once
#include <string>
#include <unordered_map>

using namespace std;

struct Coordinates {
    double lat = 0.0;
    double lon = 0.0;

    Coordinates() = default;
    Coordinates(double lat_, double lon_) : lat(lat_), lon(lon_) {}
};

class Server {
public:
    string name;
    Coordinates coord{};
    bool online = true;

    unordered_map<string, string> cache;

    long long requestsServed = 0;

public:
    Server() = default;
    Server(string n, Coordinates c, bool on = true);

    bool isOnline() const;
    void setOnline(bool on);

    bool cacheHas(const string& file) const;
    string getFromCache(const string& file) const;
    void putInCache(const string& file, const string& content);
};
