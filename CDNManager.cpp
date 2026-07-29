#include "CDNManager.h"
#include "Utils.h"

#include <iostream>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <limits>

using namespace std;

CDNManager::CDNManager() {
    loadDefaultData();
}

void CDNManager::loadDefaultData() {
    edges.clear();
    edges.emplace_back("Mumbai", Coordinates{18.5204, 73.8567}, true);
    edges.emplace_back("Delhi", Coordinates{28.7041, 77.1025}, true);
    edges.emplace_back("Chennai", Coordinates{13.0827, 80.2707}, true);

    serverRequestCount.clear();
    for (auto& e : edges) serverRequestCount[e.name] = 0;

    cities.clear();
    cities["Pune"] = Coordinates{18.5204, 73.8567};
    cities["Mumbai"] = Coordinates{18.5204, 73.8567};
    cities["Delhi"] = Coordinates{28.7041, 77.1025};
    cities["Chennai"] = Coordinates{13.0827, 80.2707};
    cities["Bengaluru"] = Coordinates{12.9716, 77.5946};
    cities["Hyderabad"] = Coordinates{17.3850, 78.4867};
    cities["Kolkata"] = Coordinates{22.5726, 88.3639};
    cities["Jaipur"] = Coordinates{26.9124, 75.7873};

    origin.files.clear();
    origin.files["hello.txt"] = "Hello World";
    origin.files["image.png"] = "Image Data (binary simulated)";
    origin.files["video.mp4"] = "Video Data (binary simulated)";

    // reset stats
    totalRequests = 0;
    cacheHits = 0;
    cacheMisses = 0;
    totalLatencyMs = 0;
}

void CDNManager::printMenu() const {
   
    cout << "SIMPLE CDN SIMULATOR\n";
   
    cout << "1. Request File\n";
    cout << "2. Toggle Server\n";
    cout << "3. View Cache\n";
    cout << "4. Server Status\n";
    cout << "5. Statistics\n";
    cout << "6. Exit\n";
    cout << "Enter choice: ";
}

int CDNManager::readInt() const {
    int x;
    while (!(cin >> x)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Enter an integer: ";
    }
    return x;
}

int CDNManager::findNearestOnlineEdgeIndex(const Coordinates& clientCoord) const {
    vector<pair<double, int>> distList;

    for (int i = 0; i < (int)edges.size(); i++) {
        if (!edges[i].isOnline()) continue;
        double d = haversineKm(clientCoord, edges[i].coord);
        distList.push_back({d, i});
    }

    if (distList.empty()) return -1;

    sort(distList.begin(), distList.end(),
         [](const pair<double, int>& p1, const pair<double, int>& p2) { return p1.first < p2.first; });

    return distList.front().second;
}

void CDNManager::updateStatsOnServe(const string& servedBy, int latencyMs, bool hit) {
    totalRequests++;
    totalLatencyMs += latencyMs;
    serverRequestCount[servedBy]++;

    if (hit) cacheHits++;
    else cacheMisses++;
}

void CDNManager::requestFileFlow() {
    cout << "\nAvailable cities:\n";
    vector<string> cityKeys;
    cityKeys.reserve(cities.size());
    for (auto& kv : cities) cityKeys.push_back(kv.first);
    sort(cityKeys.begin(), cityKeys.end());

    for (int i = 0; i < (int)cityKeys.size(); i++) {
        cout << i + 1 << ". " << cityKeys[i] << "\n";
    }

    cout << "Select city by name (type exactly): ";
    string city;
    cin >> city;

    auto itCity = cities.find(city);
    if (itCity == cities.end()) {
        cout << "Unknown city: " << city << "\n";
        return;
    }
    Coordinates clientCoord = itCity->second;

    cout << "\nAvailable files at Origin:\n";
    for (auto& kv : origin.files) cout << " - " << kv.first << "\n";

    cout << "Enter file name to request: ";
    string file;
    cin >> file;

    if (!origin.hasFile(file)) {
        cout << "Origin does not have file: " << file << "\n";
        return;
    }

    int edgeIdx = findNearestOnlineEdgeIndex(clientCoord);
    if (edgeIdx < 0) {
        cout << "No CDN edge servers are online. (All offline)\n";
        return;
    }

    Server& chosen = edges[edgeIdx];

    double distKm = haversineKm(clientCoord, chosen.coord);
    int latencyMs = latencyMsForDistanceKm(distKm);

    bool hit = chosen.cacheHas(file);
    if (hit) {
        string content = chosen.getFromCache(file);
        chosen.requestsServed++;
        updateStatsOnServe(chosen.name, latencyMs, true);

        cout << "\n[Cache HIT] Served by " << chosen.name
             << " | Distance: " << fixed << setprecision(1) << distKm << " km"
             << " | Latency: " << latencyMs << " ms\n";
        cout << "Response: " << content << "\n";
    } else {
        string content = origin.getFile(file);
        chosen.putInCache(file, content);
        chosen.requestsServed++;
        updateStatsOnServe(chosen.name, latencyMs, false);

        cout << "\n[Cache MISS] Served by " << chosen.name
             << " | Distance: " << fixed << setprecision(1) << distKm << " km"
             << " | Latency: " << latencyMs << " ms\n";
        cout << "Fetched from Origin and stored in " << chosen.name << " cache.\n";
        cout << "Response: " << content << "\n";
    }
}

void CDNManager::toggleServerFlow() {
    cout << "\nChoose server to toggle:\n";
    for (int i = 0; i < (int)edges.size(); i++) {
        cout << i + 1 << ". " << edges[i].name
             << " [" << (edges[i].online ? "ONLINE" : "OFFLINE") << "]\n";
    }

    cout << "Enter server number: ";
    int c = readInt();

    if (c < 1 || c > (int)edges.size()) {
        cout << "Invalid server choice.\n";
        return;
    }

    Server& s = edges[c - 1];
    s.setOnline(!s.isOnline());

    cout << s.name << " is now " << (s.isOnline() ? "ONLINE" : "OFFLINE") << ".\n";
}

void CDNManager::viewCacheFlow() {
    cout << "\nView cache for which server?\n";
    for (int i = 0; i < (int)edges.size(); i++) {
        cout << i + 1 << ". " << edges[i].name
             << " [" << (edges[i].online ? "ONLINE" : "OFFLINE") << "]\n";
    }

    cout << "Enter server number: ";
    int c = readInt();

    if (c < 1 || c > (int)edges.size()) {
        cout << "Invalid server choice.\n";
        return;
    }

    Server& s = edges[c - 1];

    cout << "\nCache contents of " << s.name << ":\n";
    if (s.cache.empty()) {
        cout << "(Empty)\n";
        return;
    }

    for (auto& kv : s.cache) {
        cout << " - " << kv.first << "\n";
    }
}

void CDNManager::serverStatusFlow() {
    cout << "\nServer Status:\n";
    for (auto& s : edges) {
        cout << "- " << s.name
             << " | " << (s.online ? "ONLINE" : "OFFLINE")
             << " | Requests served: " << s.requestsServed
             << " | Cache size: " << s.cache.size()
             << "\n";
    }
}

void CDNManager::statisticsFlow() {
    cout << "\n===============================\n";
    cout << "Statistics\n";
    cout << "===============================\n";

    cout << "Total Requests: " << totalRequests << "\n";
    cout << "Cache Hits:     " << cacheHits << "\n";
    cout << "Cache Misses:  " << cacheMisses << "\n";

    double hitRatio = 0.0;
    if (totalRequests > 0) hitRatio = (double)cacheHits / (double)totalRequests * 100.0;

    cout << "Hit Ratio:      " << fixed << setprecision(1) << hitRatio << "%\n";

    double avgLatency = 0.0;
    if (totalRequests > 0) avgLatency = (double)totalLatencyMs / (double)totalRequests;

    cout << "Average Latency: " << fixed << setprecision(1) << avgLatency << " ms\n";

    string mostUsed = "N/A";
    long long best = -1;
    for (auto& kv : serverRequestCount) {
        if (kv.second > best) {
            best = kv.second;
            mostUsed = kv.first;
        }
    }

    cout << "Most Used Server: " << mostUsed << "\n";

    cout << "\nServer-wise Requests:\n";
    for (auto& s : edges) {
        cout << " - " << s.name << ": " << serverRequestCount.at(s.name) << "\n";
    }

    cout << "\nExport these statistics to CSV? (y/n): ";
    char ans;
    cin >> ans;
    if (ans == 'y' || ans == 'Y') {
        exportStatisticsCSV("cdn_statistics.csv");
    }
}

void CDNManager::exportStatisticsCSV(const string& filename) const {
    double hitRatio = 0.0;
    if (totalRequests > 0) hitRatio = (double)cacheHits / (double)totalRequests * 100.0;

    double avgLatency = 0.0;
    if (totalRequests > 0) avgLatency = (double)totalLatencyMs / (double)totalRequests;

    string mostUsed = "N/A";
    long long best = -1;
    for (auto& kv : serverRequestCount) {
        if (kv.second > best) {
            best = kv.second;
            mostUsed = kv.first;
        }
    }

    bool writeHeader = false;
    {
        ifstream fin(filename);
        writeHeader = !fin.good() || fin.peek() == ifstream::traits_type::eof();
    }

    ofstream fout(filename, ios::app);
    if (!fout.is_open()) {
        cout << "Failed to open CSV file: " << filename << "\n";
        return;
    }

    if (writeHeader) {
        fout << "timestamp,totalRequests,cacheHits,cacheMisses,hitRatioPercent,avgLatencyMs,mostUsedServer\n";
    }

    fout << "\"" << currentTimestampString() << "\""
         << "," << totalRequests
         << "," << cacheHits
         << "," << cacheMisses
         << "," << fixed << setprecision(1) << hitRatio
         << "," << fixed << setprecision(1) << avgLatency
         << "," << mostUsed
         << "\n";

    fout.close();
    cout << "Statistics exported to " << filename << "\n";
}

void CDNManager::run() {
    while (true) {
        printMenu();
        int choice = readInt();

        switch (choice) {
            case 1: requestFileFlow(); break;
            case 2: toggleServerFlow(); break;
            case 3: viewCacheFlow(); break;
            case 4: serverStatusFlow(); break;
            case 5: statisticsFlow(); break;
            case 6:
                cout << "Exiting CDN simulator.\n";
                return;
            default:
                cout << "Invalid choice. Try again.\n";
        }
    }
}