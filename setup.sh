#!/bin/bash

set -e

# Проверка прав root (для установки в системные каталоги)
if [ "$(id -u)" -ne 0 ]; then
    echo "Этот скрипт должен запускаться с правами root" >&2
    exit 1
fi

OS=$(uname -s)
ARCH=$(uname -m)

echo "Установка CryptoApp на $OS $ARCH..."

# Установка системных зависимостей (пример, можно расширять)
if [ "$OS" = "Linux" ]; then
    if [ -f /etc/debian_version ]; then
        apt-get update
        apt-get install -y build-essential cmake
    elif [ -f /etc/fedora-release ]; then
        dnf groupinstall -y "Development Tools"
        dnf install -y cmake
    elif [ -f /etc/arch-release ]; then
        pacman -Sy --noconfirm base-devel cmake
    else
        echo "Пожалуйста, установите вручную gcc, make и cmake"
    fi
elif [ "$OS" = "Darwin" ]; then
    if ! xcode-select -p &>/dev/null; then
        echo "Установка Xcode Command Line Tools..."
        xcode-select --install
        until xcode-select -p &>/dev/null; do sleep 5; done
    fi
    if ! command -v brew &>/dev/null; then
        echo "Установка Homebrew..."
        /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
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
INSTALL_PREFIX=${INSTALL_PREFIX:-/usr/local}
echo "Установка приложения в: $INSTALL_PREFIX"

# Переходим в корень проекта (предполагается, что скрипт запускается из корня)
PROJECT_ROOT=$(pwd)

# Создаем и переходим в папку сборки
mkdir -p build
cd build

# Генерируем Makefile с указанием пути установки
cmake .. -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX"

# Определяем число потоков для make
if command -v nproc &>/dev/null; then
    NUM_PROC=$(nproc)
elif [[ "$OS" == "Darwin" ]]; then
    NUM_PROC=$(sysctl -n hw.ncpu)
else
    NUM_PROC=1
fi

# Сборка
make -j"$NUM_PROC"

# Установка через cmake install (воспользуемся встроенным механизмом)
make install

# Проверка установки конфигурации в /etc
if [ ! -f /etc/cryptoapp/config.cfg ]; then
    echo "Копирование конфигурации в /etc/cryptoapp/config.cfg"
    mkdir -p /etc/cryptoapp
    cp "$PROJECT_ROOT/config.cfg.example" /etc/cryptoapp/config.cfg
fi

# Обновление кэша библиотек для Linux
if [ "$OS" = "Linux" ]; then
    ldconfig
fi

echo -e "\n\033[32mУстановка завершена успешно!\033[0m"
echo "Приложение установлено в: $INSTALL_PREFIX/bin/cryptoapp"
echo "Библиотека: $INSTALL_PREFIX/lib/libMagicSquareLib.*"
echo "Конфигурация: /etc/cryptoapp/config.cfg"
echo "Документация: $INSTALL_PREFIX/share/doc/cryptoapp"
echo ""
echo "Для запуска приложения введите: cryptoapp"
