#include <iostream>
#include <string>

#include "../../include/files_utils/FilesUtils.h"

using namespace std;

// запись текста в файл
void writeTextToFile(const wstring& text, string& filePath) {
    try {
        wofstream outputFile(filePath, ios::binary);
        if (!outputFile.is_open()) {
            throw runtime_error("Не удалось открыть файл для записи: " +
                                filePath);
        }

        outputFile.imbue(locale(locale(), new codecvt_utf8<wchar_t>));

        outputFile << text;

        wcout.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
        wcout << L"Файл успешно сохранён: " << filePath.c_str() << endl;
    } catch (const exception& ex) {
        cerr << "Ошибка при записи в файл: " << ex.what() << endl;
    }
}

// получение текста с консоли
wstring getTextFromConsole() {
    wcin.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
    wcout.imbue(locale(locale(), new codecvt_utf8<wchar_t>));

    wcin.ignore(numeric_limits<streamsize>::max(), L'\n');

    wstring inputLine;
    getline(wcin, inputLine);

    return inputLine;
}

// полуение текста из файла
wstring getTextFromFile(const string& filePath) {
    try {
        namespace fs = std::filesystem;

        if (!fs::exists(filePath)) {
            throw runtime_error("Файл не найден: " + filePath);
        }

        wifstream inputFile(filePath, ios::binary);
        if (!inputFile.is_open()) {
            throw runtime_error("Не удалось открыть файл: " + filePath);
        }

        inputFile.imbue(locale(locale(), new codecvt_utf8<wchar_t>));

        wstring fileContent;
        wchar_t ch;
        while (inputFile.get(ch)) {
            fileContent.push_back(ch);
        }

        if (fileContent.empty()) {
            throw runtime_error(
                "Файл пустой или содержит некорректные символы: " + filePath);
        }

        return fileContent;
    } catch (const exception& ex) {
        wcerr << L"Ошибка при чтении файла: " << ex.what() << endl;
        return L"";
    }
}

// получение текста из бинарного файла
vector<uint8_t> readBytesFromFile(const string& filePath) {
    ifstream inputFile(filePath, ios::binary);
    if (!inputFile.is_open()) {
        return {};
    }

    inputFile.seekg(0, ios::end);
    streamsize fileSize = inputFile.tellg();
    inputFile.seekg(0, ios::beg);

    vector<uint8_t> byteBuffer(fileSize);
    if (!inputFile.read(reinterpret_cast<char*>(byteBuffer.data()), fileSize)) {
        return {};
    }

    return byteBuffer;
}

// запись в бинарном виде
bool writeBytesToFile(const vector<uint8_t>& bytes, const string& filePath) {
    ofstream outputFile(filePath, ios::binary);
    if (!outputFile.is_open()) {
        wcerr << L"Ошибка: не удалось открыть файл для записи: "
              << filePath.c_str() << endl;
        return false;
    }

    outputFile.write(reinterpret_cast<const char*>(bytes.data()),
                     static_cast<streamsize>(bytes.size()));
    outputFile.close();

    return true;
}
