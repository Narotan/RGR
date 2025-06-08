#ifndef COLUMNAR_TRANSPOSITION_H
#define COLUMNAR_TRANSPOSITION_H

#include <algorithm>
#include <cwctype>
#include <locale>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

// шифрование
wstring encryptColumnarTransposition(const wstring& text,
                                     const wstring& key);

// расшифрование
wstring decryptColumnarTransposition(const wstring& cipherText,
                                     const wstring& key);

// генерация ключа
wstring generateRandomTranspositionKey(int minLength = 5, int maxLength = 12);

#endif