#include "../../include/encryption/HillCipher.h"

using namespace std;

static wstring_convert<codecvt_utf8_utf16<wchar_t>, wchar_t> converter;

// конвертация wstring в vector<uint8_t> с utf-8 кодировкой
vector<uint8_t> wstringToUtf8Bytes(const wstring& wideStr) {
    string utf8Str = converter.to_bytes(wideStr);
    vector<uint8_t> bytes(utf8Str.begin(), utf8Str.end());
    return bytes;
}

// конвертация vector<uint8_t> с utf-8 в wstring
wstring utf8BytesToWString(const vector<uint8_t>& bytes) {
    string utf8Str(bytes.begin(), bytes.end());
    wstring wideStr = converter.from_bytes(utf8Str);
    return wideStr;
}

// вычисление обратного по модулю 256 с помощью расширенного алгоритма евклида
int modInverse256(int a) {
    a &= 0xFF;
    if (a == 0) return -1;

    int m = 256;
    int m0 = m, y = 0, x = 1;

    while (a > 1) {
        int q = a / m;
        int t = m;

        m = a % m;
        a = t;
        t = y;

        y = x - q * y;
        x = t;
    }

    if (x < 0) x += m0;

    return x;
}

// проверка обратимости 2×2 матрицы по модулю 256 (gcd(det, 256) == 1)
bool isInvertible2x2Mod256(const vector<vector<int>>& matrix) {
    int a = matrix[0][0] & 0xFF;
    int b = matrix[0][1] & 0xFF;
    int c = matrix[1][0] & 0xFF;
    int d = matrix[1][1] & 0xFF;
    int det = ((a * d) - (b * c)) & 0xFF;
    if (det < 0) det += 256;
    return std::gcd(det, 256) == 1;
}

// ввод ключа 2×2 от пользователя с проверкой корректности
optional<vector<vector<int>>> inputHillKeyBytes() {
    vector<vector<int>> key(2, vector<int>(2));
    while (true) {
        cout << "введите ключ (2×2 матрица, 4 числа 0..255 через пробел),\n"
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
            cout << "некорректный ввод. повторите.\n";
            continue;
        }

        if (a < 0 || a > 255 || b < 0 || b > 255 || c < 0 || c > 255 || d < 0 ||
            d > 255) {
            cout << "элементы должны быть в диапазоне 0..255.\n";
            continue;
        }

        key[0][0] = a;
        key[0][1] = b;
        key[1][0] = c;
        key[1][1] = d;

        if (!isInvertible2x2Mod256(key)) {
            cout << "матрица не обратима mod 256 (det не взаимно прост с 256). "
                    "повторите.\n";
            continue;
        }

        return key;
    }
}

// шифрование данных блоками 2×2 по модулю 256
vector<uint8_t> encryptHillBytes(const vector<uint8_t>& inputBytes,
                                 const vector<vector<int>>& key) {
    vector<uint8_t> data = inputBytes;
    if (data.size() % 2 != 0) {
        data.push_back(0x00);  // паддинг если нечётное число байт
    }

    vector<uint8_t> encrypted(data.size());

    for (size_t i = 0; i < data.size(); i += 2) {
        int x1 = data[i];
        int x2 = data[i + 1];

        int y1 = ((key[0][0] * x1 + key[0][1] * x2) & 0xFF);
        int y2 = ((key[1][0] * x1 + key[1][1] * x2) & 0xFF);

        encrypted[i] = static_cast<uint8_t>(y1);
        encrypted[i + 1] = static_cast<uint8_t>(y2);
    }

    return encrypted;
}

// расшифровка данных блоками 2×2 по модулю 256
vector<uint8_t> decryptHillBytes(const vector<uint8_t>& cipherBytes,
                                 const vector<vector<int>>& key) {
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

    vector<uint8_t> decrypted(data.size());

    for (size_t i = 0; i < data.size(); i += 2) {
        int y1 = data[i];
        int y2 = data[i + 1];

        int x1 = ((invKey[0][0] * y1 + invKey[0][1] * y2) & 0xFF);
        int x2 = ((invKey[1][0] * y1 + invKey[1][1] * y2) & 0xFF);

        decrypted[i] = static_cast<uint8_t>(x1);
        decrypted[i + 1] = static_cast<uint8_t>(x2);
    }

    return decrypted;
}

// генерация случайного ключа 2×2, обратимого mod 256
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
