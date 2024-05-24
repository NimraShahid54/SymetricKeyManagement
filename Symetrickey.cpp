#pragma once
#include <iostream>
#include <string>
#include <random>
#include <unordered_map>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>
using namespace std;

class SymmetricKeyManager {
public:
    string generateKey(int keyLengthBits) {
        int keyLengthBytes = (keyLengthBits + 7) / 8;
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int> distrib(0, 255);

        string key;
        for (int i = 0; i < keyLengthBytes; ++i) {
            key.push_back(static_cast<unsigned char>(distrib(gen)));
        }

        return key;
    }
};