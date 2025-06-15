#include "../../include/encryption/MagmaCipher.h"
#include <iostream>
#include <vector>
#include <random>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <stdexcept>
#include <functional>
#include <cctype>

using namespace std;

namespace {
    const uint8_t DEFAULT_SBOX[8][16] = {
        {4,10,9,2,13,8,0,14,6,11,1,12,7,15,5,3},
        {14,11,4,12,6,13,15,10,2,3,8,1,0,7,5,9},
        {5,8,1,13,10,3,4,2,14,15,12,7,6,0,9,11},
        {7,13,10,1,0,8,9,15,14,4,6,12,11,2,5,3},
        {6,12,7,1,5,15,13,8,4,10,9,14,0,3,11,2},
        {4,11,10,0,7,2,1,13,3,6,8,5,9,12,15,14},
        {13,11,4,1,3,15,5,9,0,10,14,7,6,8,2,12},
        {1,15,13,0,5,7,10,4,9,2,3,14,6,11,8,12}
    };

    uint32_t feistelFunction(const MagmaContext* ctx, uint32_t right, uint32_t roundKey) {
        uint32_t temp = right + roundKey;
        uint32_t result = 0;
        for (int i = 0; i < 8; i++) {
            uint8_t nibble = (temp >> (4 * i)) & 0xF;
            uint8_t sVal = ctx->sBox[i][nibble];
            result |= static_cast<uint32_t>(sVal) << (4 * i);
        }
        return (result << 11) | (result >> 21);
    }

    void processBlock(const MagmaContext* ctx, const uint8_t in[8], uint8_t out[8], bool encrypt) {
        uint32_t left = 0, right = 0;
        for (int i = 0; i < 4; i++) {
            left |= static_cast<uint32_t>(in[i]) << (8 * i);
            right |= static_cast<uint32_t>(in[i + 4]) << (8 * i);
        }

        for (int round = 0; round < 32; round++) {
            uint32_t roundKey;
            if (encrypt) {
                roundKey = (round < 24) ? ctx->keyParts[round % 8] : ctx->keyParts[7 - (round % 8)];
            } else {
                roundKey = (round < 8) ? ctx->keyParts[round] : ctx->keyParts[7 - (round % 8)];
            }

            uint32_t fResult = feistelFunction(ctx, right, roundKey);
            uint32_t newLeft = right;
            right = left ^ fResult;
            left = newLeft;
        }

        for (int i = 0; i < 4; i++) {
            out[i] = static_cast<uint8_t>((right >> (8 * i)) & 0xFF);
            out[i + 4] = static_cast<uint8_t>((left >> (8 * i)) & 0xFF);
        }
    }
}

void magmaInit(MagmaContext* ctx, const vector<uint8_t>& key) {
    if (key.size() != 32) {
        throw invalid_argument("Magma key must be 32 bytes");
    }
    memcpy(ctx->sBox, DEFAULT_SBOX, sizeof(DEFAULT_SBOX));
    for (int i = 0; i < 8; i++) {
        ctx->keyParts[i] = 0;
        for (int j = 0; j < 4; j++) {
            ctx->keyParts[i] |= static_cast<uint32_t>(key[i * 4 + j]) << (8 * j);
        }
    }
}

vector<uint8_t> magmaEncryptBytes(const MagmaContext* ctx, const vector<uint8_t>& input) {
    vector<uint8_t> result;
    size_t paddedSize = ((input.size() + 7) / 8) * 8;
    vector<uint8_t> padded = input;
    padded.resize(paddedSize, 0);

    for (size_t i = 0; i < paddedSize; i += 8) {
        uint8_t block[8], encrypted[8];
        memcpy(block, &padded[i], 8);
        processBlock(ctx, block, encrypted, true);
        result.insert(result.end(), encrypted, encrypted + 8);
    }
    return result;
}

vector<uint8_t> magmaDecryptBytes(const MagmaContext* ctx, const vector<uint8_t>& input) {
    if (input.size() % 8 != 0) {
        throw invalid_argument("Input size must be multiple of 8 bytes");
    }
    vector<uint8_t> result;
    for (size_t i = 0; i < input.size(); i += 8) {
        uint8_t block[8], decrypted[8];
        memcpy(block, &input[i], 8);
        processBlock(ctx, block, decrypted, false);
        result.insert(result.end(), decrypted, decrypted + 8);
    }
    // Remove padding zeros
    while (!result.empty() && result.back() == 0) {
        result.pop_back();
    }
    return result;
}

vector<uint8_t> generateRandomMagmaKey() {
    vector<uint8_t> key(32);
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 255);
    for (auto& byte : key) byte = static_cast<uint8_t>(dis(gen));
    return key;
}

vector<uint8_t> inputMagmaKey() {
    vector<uint8_t> key(32);
    cout << "Введите 32 байта ключа (hex вида 0x... или текст), или Enter для генерации: ";
    string input;
    getline(cin, input);

    if (input.empty()) {
        // Генерируем случайный ключ
        key = generateRandomMagmaKey();
        // Показываем его в hex
        cout << "Сгенерированный ключ: ";
        for (uint8_t byte : key) {
            // выводим каждый байт двумя hex-цифрами
            cout << hex << setw(2) << setfill('0') << (int)byte;
        }
        cout << dec << setfill(' ') << "\n\n";
        return key;
    }

    // Если введена строка, начинающаяся с 0x — парсим как hex
    if (input.size() >= 2 && input[0] == '0' &&
        (input[1] == 'x' || input[1] == 'X')) {
        string hexstr = input.substr(2);
        string filtered;
        for (char c : hexstr) if (!isspace((unsigned char)c)) filtered += c;
        size_t parsed = 0;
        for (size_t i = 0; i + 1 < filtered.size() && parsed < 32; i += 2) {
            key[parsed++] = static_cast<uint8_t>(
                stoi(filtered.substr(i, 2), nullptr, 16));
        }
        // Заполняем остаток нулями
        for (size_t i = parsed; i < 32; ++i) key[i] = 0;
    } else {
        // Генерируем ключ из хеша введённого текста
        hash<string> hasher;
        size_t h = hasher(input);
        for (int i = 0; i < 32; ++i) {
            key[i] = static_cast<uint8_t>((h >> ((i % 4) * 8)) & 0xFF);
        }
    }

    return key;
}
