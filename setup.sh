#!/bin/bash
set -e

# Проверка прав администратора
if [ "$(id -u)" -ne 0 ]; then
    echo "Этот скрипт должен запускаться с правами root" >&2
    exit 1
fi

OS=$(uname -s)
ARCH=$(uname -m)
echo "Установка CryptoApp на $OS $ARCH..."

# Установка системных зависимостей (пример)
echo "Установка системных зависимостей..."
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
        echo "Не удалось определить дистрибутив Linux. Установите gcc, make и cmake вручную."
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

# Интерктивный ввод пути установки
read -p "Введите путь установки (по умолчанию /usr/local): " INSTALL_PREFIX
INSTALL_PREFIX=${INSTALL_PREFIX:-/usr/local}

# Разворачиваем '~' в $HOME, если надо
if [[ "$INSTALL_PREFIX" == ~* ]]; then
    INSTALL_PREFIX="${HOME}${INSTALL_PREFIX:1}"
fi

echo "Устанавливаем в: $INSTALL_PREFIX"

# Сборка
echo "Сборка проекта..."
mkdir -p build && cd build
cmake .. 
# Определяем число потоков
if command -v nproc &>/dev/null; then
    JOBS=$(nproc)
elif [[ "$OS" == "Darwin" ]]; then
    JOBS=$(sysctl -n hw.ncpu)
else
    JOBS=1
fi
make -j"$JOBS"

# Копирование бинарей и либ
echo "Копирование файлов в $INSTALL_PREFIX..."
mkdir -p "$INSTALL_PREFIX/bin"
install -m755 "bin/cryptoapp" "$INSTALL_PREFIX/bin/cryptoapp"

mkdir -p "$INSTALL_PREFIX/lib"
install -m755 lib/libMagicSquareLib.* "$INSTALL_PREFIX/lib/"

# Документация
echo "Копирование документации..."
mkdir -p "$INSTALL_PREFIX/share/doc/cryptoapp"
install -m644 ../README.md "$INSTALL_PREFIX/share/doc/cryptoapp/README.md"
install -m644 ../LICENSE    "$INSTALL_PREFIX/share/doc/cryptoapp/LICENSE"

# Конфиг
echo "Установка конфига в /etc/cryptoapp..."
install -d /etc/cryptoapp
if [ ! -f /etc/cryptoapp/config.cfg ]; then
    install -m644 ../config.cfg.example /etc/cryptoapp/config.cfg
fi

# Для Linux обновляем кэш библиотек
if [ "$OS" = "Linux" ]; then
    ldconfig
fi

echo -e "\n\033[32mУстановка завершена успешно!\033[0m"
echo "  Бинарь: $INSTALL_PREFIX/bin/cryptoapp"
echo "  Библиотека: $INSTALL_PREFIX/lib/libMagicSquareLib.*"
echo "  Конфиг: /etc/cryptoapp/config.cfg"
echo "  Документация: $INSTALL_PREFIX/share/doc/cryptoapp"
echo -e "\nЗапускайте просто: cryptoapp\n"
