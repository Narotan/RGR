#ifndef HILL_CIPHER_BYTES_H
#define HILL_CIPHER_BYTES_H

#include <codecvt>
#include <cstdint>
#include <iostream>
#include <locale>
#include <numeric>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

// конвертация wstring -> UTF
vector<uint8_t> wstringToUtf8Bytes(const std::wstring& w);

// Наооборот 
wstring utf8BytesToWString(const std::vector<uint8_t>& bytes);

// вводит ключ 2×2, проверяет обратимость mod 256
optional<vector<vector<int>>> inputHillKeyBytes();

// шифрует байты блоками по 2 
vector<uint8_t> encryptHillBytes(const vector<uint8_t>& rawBytes,
                                 const vector<vector<int>>& key);

// джешифрует байты блоками по 2 
vector<uint8_t> decryptHillBytes(const vector<uint8_t>& cipherBytes,
                                 const vector<vector<int>>& key);

vector<vector<int>> generateRandomHillKey();

bool isInvertible2x2Mod256(const vector<vector<int>>& mat);

#endif
