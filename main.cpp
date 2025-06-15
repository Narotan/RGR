#include <algorithm>
#include <iostream>
#include <limits>
#include <locale>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "encryption/ColumnarTransposition.h"
#include "encryption/MagicSquareCipher.h"
#include "files_utils/FilesUtils.h"
#include "include/encryption/HillCipher.h"
#include "include/encryption/MagmaCipher.h"
#include "menu/Menu.h"

using namespace std;

// выбор шифра
enum Method {
    MAGIC_SQUARE = 1,
    HILL_CIPHER = 2,
    COLUMNAR_TRANSPOSITION = 3,
    MAGMA_CIPHER = 4,
    EXIT_METHOD = 5
};
// выбор объекта действий 
enum ObjectType { INPUT_CONSOLE = 1, INPUT_FILE = 2, EXIT_OBJECT = 3 };

// выбор действия 
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
                cout << "Источник: " << (object == INPUT_CONSOLE ? "Консоль" : "Файл") << endl;
                cout << "Действие: " << (action == ENCRYPT ? "Шифрование" : "Расшифровка") << endl;
                cout << "Метод: ";
                switch (method) {
                    case MAGIC_SQUARE:
                        cout << "Magic Square";
                        break;
                    case HILL_CIPHER:
                        cout << "Hill Cipher";
                        break;
                    case COLUMNAR_TRANSPOSITION:
                        cout << "Column Transporition";
                        break;
                    case MAGMA_CIPHER:
                        cout << "Magma Cipher";
                        break;
                    default:
                        cout << "Неизвестно";
                        break;
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

                    if (action == ENCRYPT) {
                        inputBytes = readBytesFromFile(sourcePath);
                        if (inputBytes.empty()) {
                            wcerr << L"Не удалось получить содержимое файла." << endl;
                            cout << "\nНажмите Enter для возврата в меню..." << endl;
                            cin.get();
                            continue;
                        }
                        string b64 = base64Encode(inputBytes);
                        inputText = utf8ToWstring(b64);
                    } else {
                        inputText = getTextFromFile(sourcePath);
                        if (inputText.empty()) {
                            wcerr << L"Не удалось прочитать зашифрованный текст из файла." << endl;
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
                        int length = static_cast<int>(inputText.size());
                        int n = 0;
                        while (true) {
                            cout << " введите нечётный n "
                                    "(n*n>= "
                                 << length << ") или нажмите Enter для генерации :" << endl;
                            string line;
                            getline(cin, line);
                            if (line.empty()) {
                                n = generateMagicSquareKey(length);
                                cout << "Ключ: " << n << endl;
                                break;
                            }
                            try {
                                n = stoi(line);
                            } catch (...) {
                                cout << "Ошибка ввода." << endl;
                                continue;
                            }
                            if (n % 2 == 0 || n * n < length) {
                                cout << "n должен быть нечётным и n*n>=length." << endl;
                                continue;
                            }
                            break;
                        }
                        if (action == ENCRYPT) {
                            processedText = encryptWithMagicSquare(inputText, n);
                            outputBytes = wstringToUtf8Bytes(processedText);
                        } else {
                            processedText = decryptWithMagicSquare(inputText, n);
                            string tmp = wstringToUtf8(processedText);
                            outputBytes = base64Decode(tmp);
                        }
                        break;
                    }
                    case HILL_CIPHER: {
                        vector<vector<int>> key;
                        while (true) {
                            cout << "введите матрицу размером 2 * 2, 4 числа через пробел (0-255) или нажмите Enter для генерации:" << endl;
                            string line;
                            getline(cin, line);
                            if (line.empty()) {
                                key = generateRandomHillKey();
                                break;
                            }
                            istringstream iss(line);
                            int a, b, c, d;
                            if (iss >> a >> b >> c >> d) {
                                key = {{a, b}, {c, d}};
                                if (!isInvertible2x2Mod256(key)) {
                                    cout << "Матрица не обратима." << endl;
                                    continue;
                                }
                                break;
                            }
                            cout << "Некорректный ввод." << endl;
                        }
                        if (inputBytes.empty()) {
                            string tmp = wstringToUtf8(inputText);
                            inputBytes = base64Decode(tmp);
                        }
                        if (action == ENCRYPT) {
                            auto enc = encryptHillBytes(inputBytes, key);
                            string b64 = base64Encode(enc);
                            processedText = utf8ToWstring(b64);
                            outputBytes = wstringToUtf8Bytes(processedText);
                        } else {
                            string tmp = wstringToUtf8(inputText);
                            auto dec = decryptHillBytes(base64Decode(tmp), key);
                            outputBytes = dec;
                        }
                        break;
                    }
                    case COLUMNAR_TRANSPOSITION: {
                        wstring key;
                        while (true) {
                            cout << "Введите ключ из символов (5-12 символов) или нажмите Enter для генерации:" << endl;
                            wstring line;
                            getline(wcin, line);
                            if (line.empty()) {
                                key = generateRandomTranspositionKey();
                                break;
                            }
                            key = line;
                            break;
                        }
                        wcout << L"Key: " << key << endl;
                        if (action == ENCRYPT) {
                            processedText = encryptColumnarTransposition(inputText, key);
                            outputBytes = wstringToUtf8Bytes(processedText);
                        } else {
                            processedText = decryptColumnarTransposition(inputText, key);
                            outputBytes = base64Decode(wstringToUtf8(processedText));
                        }
                        break;
                    }
                    case MAGMA_CIPHER: {
                        vector<uint8_t> key = inputMagmaKey();
                        MagmaContext ctx;
                        magmaInit(&ctx, key);
                        if (inputBytes.empty()) {
                            string tmp = wstringToUtf8(inputText);
                            inputBytes = base64Decode(tmp);
                        }
                        if (action == ENCRYPT) {
                            auto enc = magmaEncryptBytes(&ctx, inputBytes);
                            string b64 = base64Encode(enc);
                            processedText = utf8ToWstring(b64);
                            outputBytes = wstringToUtf8Bytes(processedText);
                        } else {
                            string tmp = wstringToUtf8(inputText);
                            auto dec = magmaDecryptBytes(&ctx, base64Decode(tmp));
                            outputBytes = dec;
                        }
                        break;
                    }
                }

                if (outputBytes.empty()) {
                    wcerr << L"Результат пуст." << endl;
                    cout << "Press Enter..." << endl;
                    cin.get();
                    continue;
                }
                cout << "Сохранить в: ";
                string save;
                getline(cin, save);
                writeBytesToFile(outputBytes, save);
                cout << "Готово. Press Enter..." << endl;
                cin.get();
            }
        }
    }
    return 0;
}
