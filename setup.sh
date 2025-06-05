#!/bin/bash

# Проверка прав администратора
if [ "$(id -u)" -ne 0 ]; then
    echo "This script must be run as root" >&2
    exit 1
fi

# Определение ОС
OS=$(uname -s)

# Сборка проекта
echo "Building project..."
mkdir -p build
cd build
cmake ..
make -j$(nproc)
cpack

# Установка
echo "Installing application..."

# Для macOS
if [ "$OS" = "Darwin" ]; then
    # Распаковка архива
    TAR_FILE=$(find . -name "CryptoApp-*-Darwin.tar.gz")
    if [ -z "$TAR_FILE" ]; then
        echo "Installation package not found!" >&2
        exit 1
    fi
    
    tar -xzf "$TAR_FILE" -C /usr/local
    
    # Настройка путей
    ln -sf /usr/local/bin/cryptoapp /usr/local/bin/crypt
    
    echo "Installed in /usr/local/bin"
    echo "Run with: crypt"

# Для Linux
elif [ "$OS" = "Linux" ]; then
    # Распаковка архива
    TAR_FILE=$(find . -name "CryptoApp-*-Linux.tar.gz")
    if [ -z "$TAR_FILE" ]; then
        echo "Installation package not found!" >&2
        exit 1
    fi
    
    tar -xzf "$TAR_FILE" -C /usr
    
    # Настройка библиотек
    ldconfig
    
    echo "Installed in /usr/bin"
    echo "Run with: cryptoapp"

else
    echo "Unsupported OS: $OS" >&2
    exit 1
fi

# Создание конфигурационного каталога
mkdir -p /etc/cryptoapp
chmod 755 /etc/cryptoapp

echo -e "\n\033[32mInstallation completed successfully!\033[0m"
echo "Documentation: /usr/share/doc/cryptoapp"