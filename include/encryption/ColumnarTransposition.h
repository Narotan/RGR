#ifndef COLUMNAR_TRANSPOSITION_H
#define COLUMNAR_TRANSPOSITION_H

#include <string>
#include <vector>
#include <stdexcept>

using namespace std;

// шифрование 
wstring encryptColumnarTransposition(const std::wstring& text, const std::wstring& key);

// расшифрование 
wstring decryptColumnarTransposition(const std::wstring& cipherText, const std::wstring& key);

// генерация ключа 
wstring generateRandomTranspositionKey(int minLength = 5, int maxLength = 12);

#endif 