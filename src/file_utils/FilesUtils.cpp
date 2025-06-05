#include <iostream>
#include <string>

#include "../../include/files_utils/FilesUtils.h"

using namespace std;

void writeTextToFile(const wstring& text, string& path) {
  try {
    wofstream file(path, ios::binary);
    if (!file.is_open()) {
      throw runtime_error("Не удалось открыть файл для записи: " + path);
    }

    // Устанавливаем UTF-8 локаль
    file.imbue(locale(locale(), new codecvt_utf8<wchar_t>));

    file << text;

    wcout.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
    wcout << L"Файл успешно сохранён: " << path.c_str() << endl;
  } catch (const exception& e) {
    cerr << "Ошибка при записи в файл: " << e.what() << endl;
  }
}

wstring getTextFromConsole() {
  // Настраиваем UTF-8 локаль для wcin и wcout
  wcin.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
  wcout.imbue(locale(locale(), new codecvt_utf8<wchar_t>));

  // Очистка ввода от мусора, если надо
  wcin.ignore(numeric_limits<streamsize>::max(), L'\n');

  wstring input;
  getline(wcin, input);  // считываем целую строку

  return input;
}

wstring getTextFromFile(const string& path) {
  try {
    namespace fs = std::filesystem;

    if (!fs::exists(path)) {
      throw runtime_error("Файл не найден: " + path);
    }

    wifstream file(path, ios::binary);
    if (!file.is_open()) {
      throw runtime_error("Не удалось открыть файл: " + path);
    }

    file.imbue(locale(locale(), new codecvt_utf8<wchar_t>));

    wstring content;
    wchar_t ch;
    while (file.get(ch)) {
      content.push_back(ch);
    }

    if (content.empty()) {
      throw runtime_error("Файл пустой или содержит некорректные символы: " +
                          path);
    }

    return content;
  } catch (const exception& e) {
    wcerr << L"Ошибка при чтении файла: " << e.what() << endl;
    return L"";
  }
}

vector<uint8_t> readBytesFromFile(const string& path) {
  ifstream file(path, ios::binary);  // открыть файл в бинарном режиме
  if (!file.is_open()) {
    return {};
  }

  file.seekg(0, ios::end);
  streamsize size = file.tellg();
  file.seekg(0, ios::beg);

  vector<uint8_t> buffer(size);
  if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
    return {};
  }

  return buffer;
}

bool writeBytesToFile(const vector<uint8_t>& bytes, const string& filepath) {
  ofstream file(filepath, ios::binary);
  if (!file.is_open()) {
    wcerr << L"Ошибка: не удалось открыть файл для записи: " << filepath.c_str()
          << endl;
    return false;
  }

  file.write(reinterpret_cast<const char*>(bytes.data()),
             static_cast<streamsize>(bytes.size()));
  file.close();

  return true;
}