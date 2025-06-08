#ifndef COLUMNAR_TRANSPOSITION_H
#define COLUMNAR_TRANSPOSITION_H

#include <string>
#include <vector>
#include <stdexcept>
#include <random>
#include <algorithm>

using namespace std;

// шифрует текст методом столбцовой перестановки
wstring encryptColumnarTransposition(const wstring& text, const wstring& key);

// дешифрует текст, зашифрованный методом столбцовой перестановки
wstring decryptColumnarTransposition(const wstring& cipherText,
                                     const wstring& key);

// генерирует случайный ключ для шифрования перестановкой от 5-12 символов 
wstring generateRandomTranspositionKey();

#endif
