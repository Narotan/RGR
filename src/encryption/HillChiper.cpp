#include "/Users/danil/education/proga/rgr/include/encryption/HillChiper.h"

using namespace std;

static wstring_convert<codecvt_utf8_utf16<wchar_t>, wchar_t> converter;

vector<uint8_t> wstringToUtf8Bytes(const wstring& w) {
  string utf8str = converter.to_bytes(w);
  vector<uint8_t> bytes(utf8str.begin(), utf8str.end());
  return bytes;
}

wstring utf8BytesToWString(const vector<uint8_t>& bytes) {
  string utf8str(bytes.begin(), bytes.end());
  wstring w = converter.from_bytes(utf8str);
  return w;
}

// Нахождение обратного числа по модулю 256 (перебором)
int modInverse256(int a) {
  a &= 0xFF;
  if (a < 0) a += 256;
  for (int x = 1; x < 256; ++x) {
    if (((a * x) & 0xFF) == 1) return x;
  }
  return -1;
}

// Проверяем, что 2×2-матрица обратима mod 256: gcd(det, 256) == 1
bool isInvertible2x2Mod256(const vector<vector<int>>& mat) {
  int a = mat[0][0] & 0xFF;
  int b = mat[0][1] & 0xFF;
  int c = mat[1][0] & 0xFF;
  int d = mat[1][1] & 0xFF;
  int det = ((a * d) - (b * c)) & 0xFF;
  if (det < 0) det += 256;
  return std::gcd(det, 256) == 1;
}

// Ввод ключа 2×2 от пользователя
optional<vector<vector<int>>> inputHillKeyBytes() {
  vector<vector<int>> key(2, vector<int>(2));
  while (true) {
    cout << "Введите ключ (2×2 матрица, 4 числа 0..255 через пробел),\n"
            "или \"cancel\" для отмены:\n";
    cout << "a11 a12 a21 a22: ";
    string line;
    getline(cin, line);
    if (line == "cancel" || line == "Cancel") {
      return nullopt;
    }
    int a, b, c, d;
    try {
      size_t pos = 0;
      a = stoi(line, &pos);
      line = line.substr(pos);
      b = stoi(line, &pos);
      line = line.substr(pos);
      c = stoi(line, &pos);
      line = line.substr(pos);
      d = stoi(line, &pos);
    } catch (...) {
      cout << "Некорректный ввод. Повторите.\n";
      continue;
    }
    if (a < 0 || a > 255 || b < 0 || b > 255 || c < 0 || c > 255 || d < 0 ||
        d > 255) {
      cout << "Элементы должны быть в диапазоне 0..255.\n";
      continue;
    }
    key[0][0] = a;
    key[0][1] = b;
    key[1][0] = c;
    key[1][1] = d;
    if (!isInvertible2x2Mod256(key)) {
      cout << "Матрица не обратима mod 256 (det не взаимно прост с 256). "
              "Повторите.\n";
      continue;
    }
    return key;
  }
}

// Шифрование байт блоками 2×2 по модулю 256
vector<uint8_t> encryptHillBytes(const vector<uint8_t>& rawBytes,
                                 const vector<vector<int>>& key) {
  vector<uint8_t> data = rawBytes;
  if (data.size() % 2 != 0) {
    data.push_back(0x00);  // паддинг 0x00, если нечётно
  }
  vector<uint8_t> out(data.size());
  for (size_t i = 0; i < data.size(); i += 2) {
    int x1 = data[i];
    int x2 = data[i + 1];
    int y1 = ((key[0][0] * x1 + key[0][1] * x2) & 0xFF);
    int y2 = ((key[1][0] * x1 + key[1][1] * x2) & 0xFF);
    out[i] = static_cast<uint8_t>(y1);
    out[i + 1] = static_cast<uint8_t>(y2);
  }
  return out;
}

// Дешифровка байт блоками 2×2 по модулю 256
vector<uint8_t> decryptHillBytes(const vector<uint8_t>& cipherBytes,
                                 const vector<vector<int>>& key) {
  // Сначала находим обратную ключевую матрицу K^{-1} mod 256
  int a = key[0][0] & 0xFF;
  int b = key[0][1] & 0xFF;
  int c = key[1][0] & 0xFF;
  int d = key[1][1] & 0xFF;
  int det = ((a * d) - (b * c)) & 0xFF;
  if (det < 0) det += 256;
  int invDet = modInverse256(det);
  vector<vector<int>> invKey(2, vector<int>(2));
  invKey[0][0] = (d * invDet) & 0xFF;
  invKey[0][1] = ((-b) * invDet) & 0xFF;
  if (invKey[0][1] < 0) invKey[0][1] += 256;
  invKey[1][0] = ((-c) * invDet) & 0xFF;
  if (invKey[1][0] < 0) invKey[1][0] += 256;
  invKey[1][1] = (a * invDet) & 0xFF;
  if (invKey[1][1] < 0) invKey[1][1] += 256;

  vector<uint8_t> data = cipherBytes;
  if (data.size() % 2 != 0) {
    data.push_back(0x00);
  }
  vector<uint8_t> out(data.size());
  for (size_t i = 0; i < data.size(); i += 2) {
    int y1 = data[i];
    int y2 = data[i + 1];
    int x1 = ((invKey[0][0] * y1 + invKey[0][1] * y2) & 0xFF);
    int x2 = ((invKey[1][0] * y1 + invKey[1][1] * y2) & 0xFF);
    out[i] = static_cast<uint8_t>(x1);
    out[i + 1] = static_cast<uint8_t>(x2);
  }
  return out;
}

vector<vector<int>> generateRandomHillKey() {
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<int> dist(0, 255);

  vector<vector<int>> key(2, vector<int>(2));
  while (true) {
    key[0][0] = dist(gen);
    key[0][1] = dist(gen);
    key[1][0] = dist(gen);
    key[1][1] = dist(gen);

    if (isInvertible2x2Mod256(key)) {
      return key;
    }
  }
}
