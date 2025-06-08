#ifndef CMAGICSQUARECIPHER_H
#define CMAGICSQUARECIPHER_H

#include <string>
#include <vector>
#include <cmath>

using namespace std;

// строим магический квадрат
vector<std::vector<int>> buildMagicSquare(int n);

// шифрование
wstring encryptWithMagicSquare(const wstring& text, int n);

// расшифровка
wstring decryptWithMagicSquare(const wstring& cipherText, int n);

// генерация ключа
int generateMagicSquareKey(int textLength);

#endif
