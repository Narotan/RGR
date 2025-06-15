#ifndef COLUMNAR_TRANSPOSITION_H
#define COLUMNAR_TRANSPOSITION_H

#include <string>
#include <vector>
#include <stdexcept>
#include <random>
#include <algorithm>

// шифрует текст методом столбцовой перестановки
std::wstring encryptColumnarTransposition(const std::wstring& text, const std::wstring& key);

// дешифрует текст, зашифрованный методом столбцовой перестановки
std::wstring decryptColumnarTransposition(const std::wstring& cipherText,
                                     const std::wstring& key);

// генерирует случайный ключ для шифрования перестановкой от 5-12 символов 
std::wstring generateRandomTranspositionKey();

#endif
