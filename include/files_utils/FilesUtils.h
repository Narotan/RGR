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
#include <vector>
#include <fstream>
#include <cstdint>

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

string base64Encode(const std::vector<uint8_t>& data);

vector<uint8_t> base64Decode(const std::string& encoded);

// перевод wstring -> UTF
string wstringToUtf8(const wstring& wstr);

// перевод UTF -> wstring
wstring utf8ToWstring(const string& str);

#endif