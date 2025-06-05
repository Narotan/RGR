#include <algorithm>
#include <cwctype>
#include <locale>
#include <random>
#include <stdexcept>
#include <vector>

using namespace std;

// вспомогательные функции
static vector<int> generateKeyPermutation(const wstring& key);
static vector<int> inversePermutation(const vector<int>& perm);

// Реализация вспомогательных функций
static vector<int> generateKeyPermutation(const wstring& key) {
  int n = key.length();
  if (n == 0) throw invalid_argument("Ключ не может быть пустым");

  vector<pair<wchar_t, int>> indexedKey;
  for (int i = 0; i < n; i++) {
    indexedKey.push_back({key[i], i});
  }

  // Сортировка с учетом повторяющихся символов
  sort(indexedKey.begin(), indexedKey.end(),
       [](const pair<wchar_t, int>& a, const pair<wchar_t, int>& b) {
         return (a.first == b.first) ? (a.second < b.second)
                                     : (a.first < b.first);
       });

  vector<int> perm(n);
  for (int rank = 0; rank < n; rank++) {
    perm[indexedKey[rank].second] = rank + 1;
  }

  return perm;
}

static vector<int> inversePermutation(const vector<int>& perm) {
  int n = perm.size();
  vector<int> inv(n);
  for (int pos = 0; pos < n; pos++) {
    for (int idx = 0; idx < n; idx++) {
      if (perm[idx] == pos + 1) {
        inv[pos] = idx;
        break;
      }
    }
  }
  return inv;
}

// Основные функции шифрования/дешифровки
wstring encryptColumnarTransposition(const wstring& text, const wstring& key) {
  if (key.empty()) throw invalid_argument("Ключ не может быть пустым");

  vector<int> perm = generateKeyPermutation(key);
  int n = perm.size();
  int L = text.length();
  int rows = (L + n - 1) / n;  // Округление вверх
  int totalChars = rows * n;

  // Дополнение текста пробелами
  wstring paddedText = text;
  paddedText.append(totalChars - L, L' ');

  // Построение таблицы
  vector<vector<wchar_t>> table(rows, vector<wchar_t>(n));
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < n; j++) {
      table[i][j] = paddedText[i * n + j];
    }
  }

  // Перестановка столбцов
  vector<vector<wchar_t>> newTable(rows, vector<wchar_t>(n));
  for (int j = 0; j < n; j++) {
    int newPos = perm[j] - 1;
    for (int i = 0; i < rows; i++) {
      newTable[i][newPos] = table[i][j];
    }
  }

  // Сборка результата
  wstring result;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < n; j++) {
      result += newTable[i][j];
    }
  }
  return result;
}

wstring decryptColumnarTransposition(const wstring& cipherText,
                                     const wstring& key) {
  if (key.empty()) throw invalid_argument("Ключ не может быть пустым");

  vector<int> perm = generateKeyPermutation(key);
  vector<int> invPerm = inversePermutation(perm);
  int n = perm.size();
  int totalChars = cipherText.length();

  if (totalChars % n != 0) {
    throw invalid_argument("Длина текста должна быть кратна длине ключа");
  }

  int rows = totalChars / n;
  vector<vector<wchar_t>> table(rows, vector<wchar_t>(n));

  // Заполнение таблицы
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < n; j++) {
      table[i][j] = cipherText[i * n + j];
    }
  }

  // Обратная перестановка
  vector<vector<wchar_t>> newTable(rows, vector<wchar_t>(n));
  for (int pos = 0; pos < n; pos++) {
    int origCol = invPerm[pos];
    for (int i = 0; i < rows; i++) {
      newTable[i][origCol] = table[i][pos];
    }
  }

  // Сборка результата
  wstring result;
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < n; j++) {
      result += newTable[i][j];
    }
  }
  return result;
}

wstring generateRandomTranspositionKey(int minLength, int maxLength) {
  static const wstring characters =
      L"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
      L"0123456789!@#$%^&*()-_=+[]{}|;:,.<>?";

  if (characters.empty()) {
    throw runtime_error("Пустой ключ не может быть");
  }

  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<> len_dist(minLength, maxLength);
  uniform_int_distribution<> char_dist(0, characters.size() - 1);

  int length = len_dist(gen);
  wstring key;

  for (int i = 0; i < length; ++i) {
    key += characters[char_dist(gen)];
  }

  return key;
}