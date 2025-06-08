#include "../../include/encryption/ColumnarTransposition.h"

using namespace std;

// генерирует перестановку столбцов на основе ключа
static vector<int> generateKeyPermutation(const wstring& key);

// вычисляет обратную перестановку для дешифрования
static vector<int> inversePermutation(const vector<int>& permutation);

static vector<int> generateKeyPermutation(const wstring& key) {
    int keyLength = key.length();
    if (keyLength == 0) {
        throw invalid_argument("Ключ не может быть пустым");
    }

    vector<pair<wchar_t, int>> keyCharsWithIndices;
    for (int i = 0; i < keyLength; i++) {
        keyCharsWithIndices.push_back(make_pair(key[i], i));
    }

    sort(keyCharsWithIndices.begin(), keyCharsWithIndices.end(),
         [](const pair<wchar_t, int>& a, const pair<wchar_t, int>& b) {
             return (a.first == b.first) ? (a.second < b.second)
                                         : (a.first < b.first);
         });

    vector<int> permutation(keyLength);
    for (int rank = 0; rank < keyLength; rank++) {
        permutation[keyCharsWithIndices[rank].second] = rank + 1;
    }

    return permutation;
}

static vector<int> inversePermutation(const vector<int>& permutation) {
    int size = permutation.size();
    vector<int> inverse(size);
    for (int position = 0; position < size; position++) {
        for (int index = 0; index < size; index++) {
            if (permutation[index] == position + 1) {
                inverse[position] = index;
                break;
            }
        }
    }
    return inverse;
}

// шифрует текст методом столбцовой перестановки
wstring encryptColumnarTransposition(const wstring& text, const wstring& key) {
    if (key.empty()) {
        throw invalid_argument("Ключ не может быть пустым");
    }

    vector<int> keyPermutation = generateKeyPermutation(key);
    int keySize = keyPermutation.size();
    int textLength = text.length();
    int rowCount = (textLength + keySize - 1) / keySize;
    int paddedLength = rowCount * keySize;

    wstring paddedText = text;
    paddedText.append(paddedLength - textLength, L' ');

    vector<vector<wchar_t>> grid(rowCount, vector<wchar_t>(keySize));
    for (int row = 0; row < rowCount; row++) {
        for (int col = 0; col < keySize; col++) {
            grid[row][col] = paddedText[row * keySize + col];
        }
    }

    vector<vector<wchar_t>> permutedGrid(rowCount, vector<wchar_t>(keySize));
    for (int col = 0; col < keySize; col++) {
        int permutedCol = keyPermutation[col] - 1;
        for (int row = 0; row < rowCount; row++) {
            permutedGrid[row][permutedCol] = grid[row][col];
        }
    }

    wstring result;
    for (int row = 0; row < rowCount; row++) {
        for (int col = 0; col < keySize; col++) {
            result += permutedGrid[row][col];
        }
    }
    return result;
}

// дешифрует текст, зашифрованный методом столбцовой перестановки
wstring decryptColumnarTransposition(const wstring& cipherText,
                                     const wstring& key) {
    if (key.empty()) {
        throw invalid_argument("Ключ не может быть пустым");
    }

    vector<int> keyPermutation = generateKeyPermutation(key);
    vector<int> inversePerm = inversePermutation(keyPermutation);
    int keySize = keyPermutation.size();
    int cipherLength = cipherText.length();

    if (cipherLength % keySize != 0) {
        throw invalid_argument("Длина текста должна быть кратна длине ключа");
    }

    int rowCount = cipherLength / keySize;
    vector<vector<wchar_t>> grid(rowCount, vector<wchar_t>(keySize));

    for (int row = 0; row < rowCount; row++) {
        for (int col = 0; col < keySize; col++) {
            grid[row][col] = cipherText[row * keySize + col];
        }
    }

    vector<vector<wchar_t>> restoredGrid(rowCount, vector<wchar_t>(keySize));
    for (int permutedCol = 0; permutedCol < keySize; permutedCol++) {
        int originalCol = inversePerm[permutedCol];
        for (int row = 0; row < rowCount; row++) {
            restoredGrid[row][originalCol] = grid[row][permutedCol];
        }
    }

    wstring result;
    for (int row = 0; row < rowCount; row++) {
        for (int col = 0; col < keySize; col++) {
            result += restoredGrid[row][col];
        }
    }
    return result;
}

// генерирует случайный ключ для шифрования перестановкой
wstring generateRandomTranspositionKey() {
    int minLength = 5; 
    int maxLength = 12;
    static const wstring validChars =
        L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
        L"0123456789!@#$%^&*()-_=+[]{}|;:,.<>?";

    if (validChars.empty()) {
        throw runtime_error("Пустой ключ не может быть");
    }

    random_device randomDevice;
    mt19937 generator(randomDevice());
    uniform_int_distribution<> lengthDistribution(minLength, maxLength);
    uniform_int_distribution<> charDistribution(0, validChars.size() - 1);

    int keyLength = lengthDistribution(generator);
    wstring randomKey;

    for (int i = 0; i < keyLength; ++i) {
        randomKey += validChars[charDistribution(generator)];
    }

    return randomKey;
}
