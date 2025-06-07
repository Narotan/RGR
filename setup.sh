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
    elif [ -f /etc/fedora-release ]; then
        dnf groupinstall -y "Development Tools"
        dnf install -y cmake
    elif [ -f /etc/arch-release ]; then
        pacman -Sy --noconfirm base-devel cmake
    else
        echo "Не удалось определить дистрибутив Linux. Установите вручную:"
        echo " - build-essential (gcc, g++, make)"
        echo " - cmake"
    fi
elif [ "$OS" = "Darwin" ]; then
    if ! xcode-select -p &>/dev/null; then
        echo "Установка Xcode Command Line Tools..."
        xcode-select --install
        until xcode-select -p &>/dev/null; do
            sleep 5
        done
    fi
    if ! command -v brew &>/dev/null; then
        echo "Установка Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
        echo 'eval "$(/opt/homebrew/bin/brew shellenv)"' >> ~/.zprofile
        eval "$(/opt/homebrew/bin/brew shellenv)"
    fi
    if ! command -v cmake &>/dev/null; then
        brew install cmake
    fi
else
    echo "Неподдерживаемая ОС: $OS" >&2
    exit 1
fi

# Запрос пути установки с дефолтом
read -p "Введите путь установки приложения (по умолчанию /usr/local): " INSTALL_PREFIX

# Если пустой ввод — дефолт
if [ -z "$INSTALL_PREFIX" ]; then
    INSTALL_PREFIX="/usr/local"
fi

# Разворачивание ~ в путь
if [[ "$INSTALL_PREFIX" == ~* ]]; then
    INSTALL_PREFIX="${HOME}${INSTALL_PREFIX:1}"
fi

echo "Установка приложения в: $INSTALL_PREFIX"

# Сборка проекта
echo "Сборка проекта..."
mkdir -p build
cd build || { echo "Не удалось перейти в папку build"; exit 1; }
cmake ..
if ! make -j"$(nproc)"; then
    echo "Ошибка сборки проекта!" >&2
    exit 1
fi

# Установка файлов
echo "Копирование файлов в систему..."
install -Dm755 bin/cryptoapp "$INSTALL_PREFIX/bin/cryptoapp"
install -Dm755 lib/libMagicSquareLib.* "$INSTALL_PREFIX/lib/"

install -d "$INSTALL_PREFIX/share/doc/cryptoapp"
install -m644 ../README.md "$INSTALL_PREFIX/share/doc/cryptoapp/"
install -m644 ../LICENSE "$INSTALL_PREFIX/share/doc/cryptoapp/"

install -d /etc/cryptoapp
if [ ! -f /etc/cryptoapp/config.cfg ]; then
    install -m644 ../config.cfg.example /etc/cryptoapp/config.cfg
fi

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
