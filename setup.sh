#!/bin/bash

# Проверка прав администратора
if [ "$(id -u)" -ne 0 ]; then
    echo "Этот скрипт должен запускаться с правами root" >&2
    exit 1
fi

# Путь установки по умолчанию
DEFAULT_INSTALL_PREFIX="/usr/local"

# Запрос пути установки у пользователя
echo "Введите путь для установки приложения (по умолчанию: $DEFAULT_INSTALL_PREFIX):"
read -r USER_INPUT

# Если пользователь ввёл пустую строку — используем путь по умолчанию
if [ -z "$USER_INPUT" ]; then
    INSTALL_PREFIX="$DEFAULT_INSTALL_PREFIX"
else
    INSTALL_PREFIX="$USER_INPUT"
fi

echo "Приложение будет установлено в: $INSTALL_PREFIX"

# Определение ОС
OS=$(uname -s)
ARCH=$(uname -m)

echo "Установка CryptoApp на $OS $ARCH..."

# --- здесь вставляем код установки зависимостей без изменений ---

# Сборка проекта
echo "Сборка проекта..."
mkdir -p build
cd build || exit 1
cmake ..
if ! make -j"$(nproc)"; then
    echo "Ошибка сборки проекта!" >&2
    exit 1
fi

# Установка файлов с выбранным путём
echo "Установка приложения в систему..."
install -Dm755 bin/cryptoapp "$INSTALL_PREFIX/bin/cryptoapp"
install -Dm755 lib/libMagicSquareLib.* "$INSTALL_PREFIX/lib/"

# Документация
install -d "$INSTALL_PREFIX/share/doc/cryptoapp"
install -m644 ../README.md "$INSTALL_PREFIX/share/doc/cryptoapp/"
install -m644 ../LICENSE "$INSTALL_PREFIX/share/doc/cryptoapp/"

# Конфигурация
install -d /etc/cryptoapp
if [ ! -f /etc/cryptoapp/config.cfg ]; then
    install -m644 ../config.cfg.example /etc/cryptoapp/config.cfg
fi

# Обновление кэша библиотек (Linux)
if [ "$OS" = "Linux" ]; then
    ldconfig
fi

echo -e "\n\033[32mУстановка завершена успешно!\033[0m"
echo "Приложение установлено в: $INSTALL_PREFIX/bin/cryptoapp"
echo "Библиотека: $INSTALL_PREFIX/lib/libMagicSquareLib.*"
echo "Конфигурация: /etc/cryptoapp/config.cfg"
echo "Документация: $INSTALL_PREFIX/share/doc/cryptoapp"
echo ""
echo "Запуск приложения:"
echo "  $ cryptoapp"
