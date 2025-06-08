#ifndef MENU_H
#define MENU_H

#include <iostream>
#include <limits>
#include <string>

// менюшка для выбора метода шифрования
void methodChoice(int& method);

// менюшка для выбора объекта шифрования
void objectChoice(int& object);

// менюшка для выбора шифрует/дешифруем
void actionChoice(int& action);

// функа которая чистит экран консоли после выбора варианта 
void clearScreen();

#endif
