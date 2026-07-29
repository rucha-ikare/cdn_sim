#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include "Server.h"
#include "OriginServer.h"

using namespace std;

class CDNManager {
public:
    CDNManager();
    void run();

private:
    vector<Server> edges;
    OriginServer origin;

    unordered_map<string, Coordinates> cities;

    long long totalRequests = 0;
    long long cacheHits = 0;
    long long cacheMisses = 0;
    long long totalLatencyMs = 0;

    unordered_map<string, long long> serverRequestCount;

private:
    void loadDefaultData();
    int findNearestOnlineEdgeIndex(const Coordinates& clientCoord) const;
    void updateStatsOnServe(const string& servedBy, int latencyMs, bool hit);
    void printMenu() const;
    int readInt() const;
    void requestFileFlow();
    void toggleServerFlow();
    void viewCacheFlow();
    void serverStatusFlow();
    void statisticsFlow();
    void exportStatisticsCSV(const string& filename) const;
};
