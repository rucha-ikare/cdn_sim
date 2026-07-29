# TODO: Make code more human-written

## Step 1: Headers (.h files) - Add `using namespace std;` and remove `std::`
- [x] Server.h
- [x] OriginServer.h
- [x] CDNManager.h
- [x] Utils.h (also remove namespace Utils wrapper)

## Step 2: Implementation (.cpp files) - Remove remaining `std::` prefixes
- [x] Server.cpp
- [x] OriginServer.cpp
- [x] CDNManager.cpp (already has `using namespace std;` but may have leftover std::)
- [x] Utils.cpp (remove namespace Utils wrapper, add using namespace std)

## Step 3: main.cpp
- [x] No std:: references but verify

