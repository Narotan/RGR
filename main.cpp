#include <iostream>
#include <limits>
#include <locale>
#include <sstream>
#include <string>
#include <random>
#include <algorithm>

#include "encryption/ColumnarTransposition.h"
#include "encryption/MagicSquareCipher.h"
#include "files_utils/FilesUtils.h"
#include "include/encryption/HillChiper.h"
#include "menu/Menu.h"

using namespace std;

enum Method { MAGIC_SQUARE = 1, HILL_CIPHER = 2, COLUMNAR_TRANSPOSITION = 3, EXIT_METHOD = 4 };
enum ObjectType { INPUT_CONSOLE = 1, INPUT_FILE = 2, EXIT_OBJECT = 3 };
enum ActionType { ENCRYPT = 1, DECRYPT = 2, EXIT_ACTION = 3 };

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    locale::global(locale("ru_RU.UTF-8"));
    
    ios::sync_with_stdio(false);
    wcin.imbue(locale());
    wcout.imbue(locale());
    wcerr.imbue(locale());

    while (true) {
        int object = 0;
        objectChoice(object);
        if (object == EXIT_OBJECT) {
            cout << "Выход из программы." << endl;
            break;
        }

        while (true) {
            int action = 0;
            actionChoice(action);
            if (action == EXIT_ACTION) break;

            while (true) {
                int method = 0;
                methodChoice(method);
                if (method == EXIT_METHOD) break;

                clearScreen();
                cout << "Источник: " << (object == INPUT_CONSOLE ? "Консоль" : "Файл")
                     << endl;
                cout << "Действие: "
                     << (action == ENCRYPT ? "Шифрование" : "Расшифровка") << endl;
                cout << "Метод: ";
                switch (method) {
                    case MAGIC_SQUARE: cout << "Magic Square"; break;
                    case HILL_CIPHER: cout << "Hill Cipher"; break;
                    case COLUMNAR_TRANSPOSITION: cout << "Метод перестановок"; break;
                    default: cout << "Неизвестно"; break;
                }
                cout << endl << endl;

                wstring inputText;
                vector<uint8_t> inputBytes;
                string sourcePath;

                if (object == INPUT_CONSOLE) {
                    inputText = getTextFromConsole();
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                } else {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Введите путь до исходного файла: ";
                    getline(cin, sourcePath);

                    if (method == HILL_CIPHER) {
                        inputBytes = readBytesFromFile(sourcePath);
                        if (inputBytes.empty()) {
                            wcerr << L"Не удалось получить содержимое файла." << endl;
                            cout << "\nНажмите Enter для возврата в меню..." << endl;
                            cin.get();
                            continue;
                        }
                    } else {
                        inputText = getTextFromFile(sourcePath);
                        if (inputText.empty()) {
                            wcerr << L"Не удалось получить содержимое файла." << endl;
                            cout << "\nНажмите Enter для возврата в меню..." << endl;
                            cin.get();
                            continue;
                        }
                    }
                }

                vector<uint8_t> outputBytes;
                wstring processedText;

                switch (method) {
                    case MAGIC_SQUARE: {
                        int n = 0;
                        while (true) {
                            cout << "Нажмите Enter для автоматического выбора ключа,\n"
                                 << "или введите нечётный размер магического квадрата (n),\n"
                                 << "где n*n >= длина текста (" << inputText.size() << "): ";
                            string line;
                            getline(cin, line);

                            if (line.empty()) {
                                n = generateMagicSquareKey(static_cast<int>(inputText.size()));
                                cout << "Сгенерированный ключ: " << n << endl;
                                break;
                            }

                            try {
                                n = stoi(line);
                            } catch (...) {
                                cout << "Ошибка: введите корректное число." << endl;
                                continue;
                            }

                            if (n % 2 == 0 || n * n < static_cast<int>(inputText.size())) {
                                cout << "Ошибка: n должен быть нечётным и n*n >= длина текста." << endl;
                                continue;
                            }
                            break;
                        }

                        processedText = (action == ENCRYPT)
                                        ? encryptWithMagicSquare(inputText, n)
                                        : decryptWithMagicSquare(inputText, n);
                        break;
                    }

                    case HILL_CIPHER: {
                        vector<vector<int>> key;
                        while (true) {
                            cout << "Нажмите Enter для генерации 2×2 ключа автоматически,\n"
                                 << "или введите вручную 4 целых числа от 0 до 255:\n";
                            string line;
                            getline(cin, line);

                            if (line.empty()) {
                                key = generateRandomHillKey();
                                cout << "Сгенерированный ключ:\n";
                                for (const auto& row : key) {
                                    for (int val : row) cout << val << " ";
                                    cout << endl;
                                }
                                break;
                            } else {
                                istringstream iss(line);
                                int a, b, c, d;
                                if (iss >> a >> b >> c >> d) {
                                    key = {{a, b}, {c, d}};
                                    if (!isInvertible2x2Mod256(key)) {
                                        cout << "Матрица не обратима по модулю 256. Повторите ввод.\n";
                                        continue;
                                    }
                                    break;
                                } else {
                                    cout << "Некорректный ввод. Повторите.\n";
                                }
                            }
                        }

                        if (inputBytes.empty()) {
                            inputBytes = wstringToUtf8Bytes(inputText);
                        }

                        outputBytes = (action == ENCRYPT)
                                      ? encryptHillBytes(inputBytes, key)
                                      : decryptHillBytes(inputBytes, key);
                        break;
                    }

                    case COLUMNAR_TRANSPOSITION: {
                      wstring transpositionKey;
                      while(true) {
                        // Всегда запрашиваем ключ, независимо от источника данных
                        cout << "Нажмите Enter для автоматической генерации ключа,\n"
                              << "или введите ключ (любые символы, рекомендуется длина 5-12 символов): ";
                        
                        wstring inputLine;
                        getline(wcin, inputLine);
                        if (inputLine.size() == 0) {
                            transpositionKey = generateRandomTranspositionKey();
                            break;
                        } else {
                            transpositionKey = inputLine;
                            break;
                        }
                      }

                      wcout << L"Сгенерированный ключ: " << transpositionKey << endl;

                      if (action == ENCRYPT) {
                          processedText = encryptColumnarTransposition(inputText, transpositionKey);
                      } else {
                          try {
                              processedText = decryptColumnarTransposition(inputText, transpositionKey);
                          } catch (const invalid_argument& e) {
                              wcerr << L"Ошибка: " << e.what() << endl;
                              processedText = L"";
                          }
                      }
                      
                      break;
                  }

                } // конец switch(method)

                // Проверка результата для всех методов
                bool resultEmpty = false;
                if (method == HILL_CIPHER) {
                    resultEmpty = outputBytes.empty();
                } else {
                    resultEmpty = processedText.empty();
                }

                if (resultEmpty) {
                    wcerr << L"Не удалось выполнить операцию. Результат пуст." << endl;
                    cout << "\nНажмите Enter для возврата в меню..." << endl;
                    cin.get();
                    continue;
                }

                cout << "Введите путь для сохранения результата: ";
                string savePath;
                getline(cin, savePath);

                if (method == HILL_CIPHER) {
                    writeBytesToFile(outputBytes, savePath);
                } else {
                    writeTextToFile(processedText, savePath);
                }

                cout << "\nРезультат успешно сохранён. Нажмите Enter для возврата в меню..." << endl;
                cin.get();
            } 
        } 
    } 

    return 0;
}