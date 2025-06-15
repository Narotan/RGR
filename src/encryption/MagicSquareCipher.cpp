#include "../../include/encryption/MagicSquareCipher.h"

using namespace std;

// строит магический квадрат нечётного порядка
vector<vector<int>> buildMagicSquare(int size) {
    vector<vector<int>> square(size, vector<int>(size, 0));
    int value = 1;
    int row = 0, col = size / 2;

    while (value <= size * size) {
        square[row][col] = value++;
        int nextRow = (row - 1 + size) % size;
        int nextCol = (col + 1) % size;

        if (square[nextRow][nextCol] != 0) {
            row = (row + 1) % size;
        } else {
            row = nextRow;
            col = nextCol;
        }
    }

    return square;
}

// шифрует текст с помощью магического квадрата
wstring encryptWithMagicSquare(const wstring& inputText, int size) {
    vector<vector<int>> square = buildMagicSquare(size);
    vector<wchar_t> encoded(size * size, L' ');

    for (int pos = 0;
         pos < static_cast<int>(inputText.size()) && pos < size * size; ++pos) {
        int r = pos / size;
        int c = pos % size;
        int targetIdx = square[r][c] - 1;

        if (targetIdx < static_cast<int>(encoded.size())) {
            encoded[targetIdx] = inputText[pos];
        }
    }

    return wstring(encoded.begin(), encoded.end());
}

// расшифровывает текст, зашифрованный магическим квадратом
wstring decryptWithMagicSquare(const wstring& encryptedText, int size) {
    vector<vector<int>> square = buildMagicSquare(size);
    vector<wchar_t> decoded(size * size, L' ');

    for (int pos = 0; pos < size * size; ++pos) {
        int r = pos / size;
        int c = pos % size;
        int sourceIdx = square[r][c] - 1;

        if (sourceIdx < static_cast<int>(encryptedText.size())) {
            decoded[pos] = encryptedText[sourceIdx];
        } else {
            decoded[pos] = L' ';
        }
    }

    return wstring(decoded.begin(), decoded.end());
}

// генерирует минимальное нечётное n, чтобы n×n >= длина текста
int generateMagicSquareKey(int textLength) {
    if (textLength <= 0) {
        return 1;
    }

    int side = static_cast<int>(ceil(sqrt(static_cast<double>(textLength))));
    if (side % 2 == 0) {
        side++;
    }

    return side;
}