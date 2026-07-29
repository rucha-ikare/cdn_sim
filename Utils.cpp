#include "Utils.h"
#include <cmath>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>

using namespace std;

double deg2rad(double deg) {
    return deg * 3.141592653589793 / 180.0;
}

double haversineKm(const Coordinates& a, const Coordinates& b) {
    double R = 6371.0;
    double dLat = deg2rad(b.lat - a.lat);
    double dLon = deg2rad(b.lon - a.lon);

    double lat1 = deg2rad(a.lat);
    double lat2 = deg2rad(b.lat);

    double h = sin(dLat / 2) * sin(dLat / 2) +
               cos(lat1) * cos(lat2) *
               sin(dLon / 2) * sin(dLon / 2);

    double c = 2 * atan2(sqrt(h), sqrt(1 - h));
    return R * c;
}

int latencyMsForDistanceKm(double distKm) {
    if (distKm <= 300.0) return 10;
    if (distKm <= 700.0) return 25;
    if (distKm <= 1200.0) return 50;
    return 90;
}

string currentTimestampString() {
    auto now = chrono::system_clock::now();
    time_t t = chrono::system_clock::to_time_t(now);

    tm timeInfo;
    // Use C-style localtime. Parentheses prevent macro expansion on MinGW.
    tm* p = (localtime)(&t);
    if (p) timeInfo = *p;

    ostringstream oss;
    oss << put_time(&timeInfo, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}
