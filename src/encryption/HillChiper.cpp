#include "/Users/danil/Documents/GitHub/RGR/include/encryption/ColumnarTransposition.h"

using namespace std;

// генерирует перестановку столбцов на основе ключа
static vector<int> generateKeyPermutation(const wstring& key);

// вычисляет обратную перестановку для дешифрования
static vector<int> inversePermutation(const vector<int>& permutation);

static vector<int> generateKeyPermutation(const wstring& key) {
    int keyLength = key.length();
    if (keyLength == 0) {
        throw invalid_argument("ключ не может быть пустым");
    }

    vector<pair<wchar_t, int>> indexedKey;
    for (int index = 0; index < keyLength; index++) {
        indexedKey.push_back(make_pair(key[index], index));
    }

    sort(indexedKey.begin(), indexedKey.end(),
         [](const pair<wchar_t, int>& a, const pair<wchar_t, int>& b) {
             return (a.first == b.first) ? (a.second < b.second)
                                         : (a.first < b.first);
         });

    vector<int> permutation(keyLength);
    for (int rank = 0; rank < keyLength; rank++) {
        permutation[indexedKey[rank].second] = rank + 1;
    }

    return permutation;
}

static vector<int> inversePermutation(const vector<int>& permutation) {
    int size = permutation.size();
    vector<int> inverse(size);
    for (int position = 0; position < size; position++) {
        for (int i = 0; i < size; i++) {
            if (permutation[i] == position + 1) {
                inverse[position] = i;
                break;
            }
        }
    }
    return inverse;
}

// шифрует текст методом столбцовой перестановки
wstring encryptColumnarTransposition(const wstring& text, const wstring& key) {
    if (key.empty()) {
        throw invalid_argument("ключ не может быть пустым");
    }

    vector<int> keyPermutation = generateKeyPermutation(key);
    int keySize = keyPermutation.size();
    int textLength = text.length();
    int rowCount = (textLength + keySize - 1) / keySize;
    int paddedLength = rowCount * keySize;

    wstring paddedText = text;
    paddedText.append(paddedLength - textLength, L' ');

    vector<vector<wchar_t>> matrix(rowCount, vector<wchar_t>(keySize));
    for (int row = 0; row < rowCount; row++) {
        for (int col = 0; col < keySize; col++) {
            matrix[row][col] = paddedText[row * keySize + col];
        }
    }

    vector<vector<wchar_t>> permutedMatrix(rowCount, vector<wchar_t>(keySize));
    for (int col = 0; col < keySize; col++) {
        int permutedCol = keyPermutation[col] - 1;
        for (int row = 0; row < rowCount; row++) {
            permutedMatrix[row][permutedCol] = matrix[row][col];
        }
    }

    wstring result;
    for (int row = 0; row < rowCount; row++) {
        for (int col = 0; col < keySize; col++) {
            result += permutedMatrix[row][col];
        }
    }
    return result;
}

// дешифрует текст, зашифрованный методом столбцовой перестановки
wstring decryptColumnarTransposition(const wstring& cipherText,
                                     const wstring& key) {
    if (key.empty()) {
        throw invalid_argument("ключ не может быть пустым");
    }

    vector<int> keyPermutation = generateKeyPermutation(key);
    vector<int> inversePerm = inversePermutation(keyPermutation);
    int keySize = keyPermutation.size();
    int cipherLength = cipherText.length();

    if (cipherLength % keySize != 0) {
        throw invalid_argument("длина текста должна быть кратна длине ключа");
    }

    int rowCount = cipherLength / keySize;
    vector<vector<wchar_t>> matrix(rowCount, vector<wchar_t>(keySize));

    for (int row = 0; row < rowCount; row++) {
        for (int col = 0; col < keySize; col++) {
            matrix[row][col] = cipherText[row * keySize + col];
        }
    }

    vector<vector<wchar_t>> restoredMatrix(rowCount, vector<wchar_t>(keySize));
    for (int permutedCol = 0; permutedCol < keySize; permutedCol++) {
        int originalCol = inversePerm[permutedCol];
        for (int row = 0; row < rowCount; row++) {
            restoredMatrix[row][originalCol] = matrix[row][permutedCol];
        }
    }

    wstring result;
    for (int row = 0; row < rowCount; row++) {
        for (int col = 0; col < keySize; col++) {
            result += restoredMatrix[row][col];
        }
    }
    return result;
}

// генерирует случайный ключ для шифрования перестановкой
wstring generateRandomTranspositionKey(int minLength, int maxLength) {
    static const wstring allowedChars =
        L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
        L"0123456789!@#$%^&*()-_=+[]{}|;:,.<>?";

    if (allowedChars.empty()) {
        throw runtime_error("пустой ключ не может быть");
    }

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> lengthDist(minLength, maxLength);
    uniform_int_distribution<> charDist(0, allowedChars.size() - 1);

    int keyLength = lengthDist(gen);
    wstring key;

    for (int i = 0; i < keyLength; ++i) {
        key += allowedChars[charDist(gen)];
    }

    return key;
}
