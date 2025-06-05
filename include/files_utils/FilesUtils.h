#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <codecvt>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <locale>
#include <stdexcept>
#include <string>

using namespace std;

// обработка текста с консоли
wstring getTextFromConsole();

// обработка текста с файла
wstring getTextFromFile(const string& path);

// запись текста в файо
void writeTextToFile(const wstring& text, string& path);

// в бинарном виде
vector<uint8_t> readBytesFromFile(const string& path);

// в бинарном виде
bool writeBytesToFile(const vector<uint8_t>& bytes, const string& filepath);

#endif
