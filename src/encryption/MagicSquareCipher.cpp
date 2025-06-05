#include <vector>

#include "../../include/encryption/MagicSquareCipher.h"

using namespace std;

// Построение магического квадрата нечётного порядка
vector<vector<int>> buildMagicSquare(int n) {
  vector<vector<int>> square(n, vector<int>(n, 0));
  int num = 1, i = 0, j = n / 2;

  while (num <= n * n) {
    square[i][j] = num++;
    int newi = (i - 1 + n) % n;
    int newj = (j + 1) % n;
    if (square[newi][newj] != 0)
      i = (i + 1) % n;
    else {
      i = newi;
      j = newj;
    }
  }
  return square;
}

// Шифрование текста с помощью магического квадрата
wstring encryptWithMagicSquare(const wstring& text, int n) {
  vector<vector<int>> square = buildMagicSquare(n);
  vector<wchar_t> buffer(n * n, L' ');

  for (int i = 0; i < text.size() && i < n * n; ++i) {
    int row = i / n;
    int col = i % n;
    int index = square[row][col] - 1;
    if (index < buffer.size()) {
      buffer[index] = text[i];
    }
  }

  return wstring(buffer.begin(), buffer.end());
}

// Расшифровка текста, зашифрованного магическим квадратом
wstring decryptWithMagicSquare(const wstring& cipherText, int n) {
  vector<vector<int>> square = buildMagicSquare(n);
  vector<wchar_t> original(n * n, L' ');

  for (int i = 0; i < n * n; ++i) {
    int row = i / n;
    int col = i % n;
    int index = square[row][col] - 1;
    if (index < cipherText.size()) {
      original[i] = cipherText[index];
    } else {
      original[i] = L' ';
    }
  }

  return wstring(original.begin(), original.end());
}

int generateMagicSquareKey(int textLength) {
  if (textLength <= 0) {
    return 1;
  }
  int n = static_cast<int>(ceil(sqrt(static_cast<double>(textLength))));
  if (n % 2 == 0) {
    n++;
  }
  return n;
}
