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

// конвертация wstring -> UTF
std::vector<uint8_t> wstringToUtf8Bytes(const std::wstring& w);

// Наооборот 
std::wstring utf8BytesToWString(const std::vector<uint8_t>& bytes);

// вводит ключ 2×2, проверяет обратимость mod 256
std::optional<std::vector<std::vector<int>>> inputHillKeyBytes();

// шифрует байты блоками по 2 
std::vector<uint8_t> encryptHillBytes(const std::vector<uint8_t>& rawBytes,
                                 const std::vector<std::vector<int>>& key);

// джешифрует байты блоками по 2 
std::vector<uint8_t> decryptHillBytes(const std::vector<uint8_t>& cipherBytes,
                                 const std::vector<std::vector<int>>& key);

// генератор ключа 
std::vector<std::vector<int>> generateRandomHillKey();

// проверка ключа
bool isInvertible2x2Mod256(const std::vector<std::vector<int>>& mat);

#endif
