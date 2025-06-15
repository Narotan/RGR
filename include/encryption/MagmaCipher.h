#ifndef MAGMACIPHER_H
#define MAGMACIPHER_H

#include <vector>
#include <cstdint>
#include <string>
#include <random>

struct MagmaContext {
    uint32_t keyParts[8];
    uint8_t sBox[8][16];
};

// инициализация контекста с ключом (32 байта)
void magmaInit(MagmaContext* ctx, const std::vector<uint8_t>& key);

// щифрование данных
std::vector<uint8_t> magmaEncryptBytes(const MagmaContext* ctx, const std::vector<uint8_t>& input);

// дешифрование данных
std::vector<uint8_t> magmaDecryptBytes(const MagmaContext* ctx, const std::vector<uint8_t>& input);

// генерация случайного ключа
std::vector<uint8_t> generateRandomMagmaKey();

// ввод ключа с консоли
std::vector<uint8_t> inputMagmaKey();

#endif