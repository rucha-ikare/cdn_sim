#pragma once
#include <string>
#include "Server.h"

using namespace std;

double haversineKm(const Coordinates& a, const Coordinates& b);
int latencyMsForDistanceKm(double distKm);
string currentTimestampString();
