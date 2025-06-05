#include "menu/Menu.h"

using namespace std;

void clearScreen() {
#ifdef _WIN32
  system("cls");
#else
  system("clear");
#endif
}

void methodChoice(int& method) {
  int choice;
  do {
    clearScreen();
    cout << "\n–––––––––––––––––– Выбор метода ––––––––––––––––––" << endl;
    cout << "1 - Шифрование с помощью МАГИЧЕСКОГО КВАДРАТА" << endl;
    cout << "2 - Шифрование с помощью метода ХИЛЛА" << endl;
    cout << "3 - Шифрование с помощью МЕТОДА ПЕРЕСТАНОВОК" << endl;
    cout << "4 - Выход" << endl;
    cout << "Выберите метод: ";
    if (!(cin >> choice)) {
      cin.clear();
      cin.ignore(numeric_limits<streamsize>::max(), '\n');
      cout << "Ошибка: введите число.\n";
      continue;
    }

    if (choice == 1 || choice == 2 || choice == 3 || choice == 4) {
      method = choice;
      return;
    } else {
      cout << "Ошибка: некорректный выбор.\n";
    }

  } while (true);
}

void objectChoice(int& object) {
  int choice;
  do {
    clearScreen();
    cout << "\n–––––––––––––––––– Выбор источника ––––––––––––––––––" << endl;
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

void actionChoice(int& action) {
  int choice;
  do {
    clearScreen();
    cout << "\n–––––––––––––––––– Выбор действия ––––––––––––––––––" << endl;
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
