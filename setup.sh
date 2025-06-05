#!/bin/bash

# Проверка прав администратора
if [ "$(id -u)" -ne 0 ]; then
    echo "Этот скрипт должен запускаться с правами root" >&2
    exit 1
fi

# Определение ОС
OS=$(uname -s)
ARCH=$(uname -m)

echo "Установка CryptoApp на $OS $ARCH..."

# Установка зависимостей
echo "Установка системных зависимостей..."
if [ "$OS" = "Linux" ]; then
    # Для Debian/Ubuntu
    if [ -f /etc/debian_version ]; then
        apt-get update
        apt-get install -y build-essential cmake
    # Для Fedora
    elif [ -f /etc/fedora-release ]; then
        dnf groupinstall -y "Development Tools"
        dnf install -y cmake
    # Для Arch
    elif [ -f /etc/arch-release ]; then
        pacman -Sy --noconfirm base-devel cmake
    else
        echo "Не удалось определить дистрибутив Linux. Установите вручную:"
        echo " - build-essential (gcc, g++, make)"
        echo " - cmake"
    fi
elif [ "$OS" = "Darwin" ]; then
    # Проверка установки Xcode Command Line Tools
    if ! xcode-select -p &>/dev/null; then
        echo "Установка Xcode Command Line Tools..."
        xcode-select --install
        # Ожидание завершения установки
        until xcode-select -p &>/dev/null; do
            sleep 5
        done
    fi
    
    # Проверка Homebrew
    if ! command -v brew &>/dev/null; then
        echo "Установка Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
        echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zprofile
        eval "$(/opt/homebrew/bin/brew shellenv)"
    fi
    
    # Установка CMake через Homebrew
    if ! command -v cmake &>/dev/null; then
        brew install cmake
    fi
else
    echo "Неподдерживаемая ОС: $OS" >&2
    exit 1
fi

# Сборка проекта
echo "Сборка проекта..."
mkdir -p build
cd build
cmake ..
if ! make -j$(nproc); then
    echo "Ошибка сборки проекта!" >&2
    exit 1
fi

# Установка файлов
echo "Установка приложения в систему..."
INSTALL_PREFIX=""
if [ "$OS" = "Linux" ]; then
    INSTALL_PREFIX="/usr/local"
elif [ "$OS" = "Darwin" ]; then
    INSTALL_PREFIX="/usr/local"
fi

# Основные файлы
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