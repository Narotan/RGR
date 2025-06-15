#ifndef CMAGICSQUARECIPHER_H
#define CMAGICSQUARECIPHER_H

#include <string>
#include <vector>
#include <cmath>

// строим магический квадрат
std::vector<std::vector<int>> buildMagicSquare(int n);

// шифрование
std::wstring encryptWithMagicSquare(const std::wstring& text, int n);

// расшифровка
std::wstring decryptWithMagicSquare(const std::wstring& cipherText, int n);

// генерация ключа
int generateMagicSquareKey(int textLength);

#endif