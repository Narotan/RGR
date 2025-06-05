cmake_minimum_required(VERSION 3.10)
project(main LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

# Разделение исходников
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
add_executable(main ${MAIN_SOURCES})

# Подключаем include/ во все цели
target_include_directories(main PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(MagicSquareLib PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Линкуем main с библиотекой
target_link_libraries(main MagicSquareLib)

# Папка для libMagicSquareLib.dylib
set_target_properties(MagicSquareLib PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
)
