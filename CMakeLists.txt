cmake_minimum_required(VERSION 3.10)
project(CryptoApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Настройка путей вывода
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# Исходные файлы
set(MAIN_SOURCES
    main.cpp
    src/file_utils/FilesUtils.cpp
    src/menu/Menu.cpp
    src/encryption/HillChiper.cpp
    src/encryption/ColumnarTransposition.cpp
)

set(MAGIC_SQUARE_SOURCES
    src/encryption/MagicSquareCipher.cpp
)

# Динамическая библиотека
add_library(MagicSquareLib SHARED ${MAGIC_SQUARE_SOURCES})

# Основной исполняемый файл
add_executable(cryptoapp ${MAIN_SOURCES})

# Подключение include-директории
target_include_directories(cryptoapp PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(MagicSquareLib PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Линковка
target_link_libraries(cryptoapp MagicSquareLib)

# Простая установка для разработчиков (опционально)
option(DEV_INSTALL "Developer installation" OFF)
if(DEV_INSTALL)
    install(TARGETS cryptoapp DESTINATION bin)
    install(TARGETS MagicSquareLib DESTINATION lib)
endif()