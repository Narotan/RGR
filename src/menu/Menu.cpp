#include "menu/Menu.h"

using namespace std;

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// выбор метода шифрования
void methodChoice(int& method) {
    int choice;
    do {
        clearScreen();
        cout << "\n–––––––––––––––––– Выбор метода ––––––––––––––––––" << endl;
        cout << "1 - Шифрование с помощью магического квадрата" << endl;
        cout << "2 - Шифрование с помощью метода Хилла" << endl;
        cout << "3 - Шифрование с помощью метода перестановок" << endl;
        cout << "4 - Шифрование с помощью ГОСТ 28147-89 (Магма)" << endl;
        cout << "5 - Назад" << endl;
        cout << "Выберите метод: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Ошибка: введите число.\n";
            continue;
        }

        if (choice >= 1 && choice <= 5) {
            method = choice;
            return;
        } else {
            cout << "Ошибка: некорректный выбор.\n";
        }

    } while (true);
}


// выбор объекта шифрования
void objectChoice(int& object) {
    int choice;
    do {
        clearScreen();
        cout << "\n–––––––––––––––––– Выбор источника ––––––––––––––––––"
             << endl;
        cout << "1 - Текст с консоли" << endl;
        cout << "2 - Путь до файла" << endl;
        cout << "3 - Назад" << endl;
        cout << "Выберите откуда шифруем/расшифровываем: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Ошибка: введите число.\n";
            continue;
        }

        if (choice >= 1 && choice <= 3) {
            object = choice;
            return;
        } else {
            cout << "Ошибка: некорректный выбор.\n";
        }

    } while (true);
}

// выбор шифровать/расшифровывать
void actionChoice(int& action) {
    int choice;
    do {
        clearScreen();
        cout << "\n–––––––––––––––––– Выбор действия ––––––––––––––––––"
             << endl;
        cout << "1 - Шифруем" << endl;
        cout << "2 - Расшифровываем" << endl;
        cout << "3 - Назад" << endl;
        cout << "Что делаем: ";

        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Ошибка: введите число.\n";
            continue;
        }

        if (choice >= 1 && choice <= 3) {
            action = choice;
            return;
        } else {
            cout << "Ошибка: некорректный выбор.\n";
        }

    } while (true);
}
