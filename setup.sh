#!/bin/bash
# setup.sh — автоматическая сборка и установка CryptoApp без конфига

set -e

# Проверка прав root
if [ "$(id -u)" -ne 0 ]; then
  echo "Ошибка: скрипт нужно запускать с правами root" >&2
  exit 1
fi

OS=$(uname -s)
echo "Установка CryptoApp на $OS..."

# 1. Установка зависимостей
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
    echo "Установите gcc, make и cmake вручную." >&2
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

# 2. Запрос пути установки
read -p "Введите путь установки (по умолчанию /usr/local): " INSTALL_PREFIX
INSTALL_PREFIX=${INSTALL_PREFIX:-/usr/local}
if [[ "$INSTALL_PREFIX" == ~* ]]; then
  INSTALL_PREFIX="${HOME}${INSTALL_PREFIX:1}"
fi
echo "Устанавливаем в: $INSTALL_PREFIX"

# 3. Сборка
echo "Сборка проекта..."
mkdir -p build
cd build
cmake .. -DCMAKE_INSTALL_PREFIX="$INSTALL_PREFIX"

# Определяем число потоков
if command -v nproc &>/dev/null; then
  JOBS=$(nproc)
elif [ "$OS" = "Darwin" ]; then
  JOBS=$(sysctl -n hw.ncpu)
else
  JOBS=1
fi

make -j"$JOBS"

# 4. Установка через CMake
echo "Установка файлов..."
make install

# 5. Обновление кэша библиотек на Linux
if [ "$OS" = "Linux" ]; then
  ldconfig
fi

echo -e "\n\033[32mУстановка завершена успешно!\033[0m"
echo "  Бинарь: $INSTALL_PREFIX/bin/cryptoapp"
echo "  Библиотеки: $INSTALL_PREFIX/lib/"
echo "  Документация: $INSTALL_PREFIX/share/doc/cryptoapp/README.md"
echo -e "\nТеперь для запуска просто выполните: cryptoapp\n"
