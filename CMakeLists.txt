cmake_minimum_required(VERSION 3.10)
project(CryptoApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Настройка RPATH для macOS ---
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
set(CMAKE_SKIP_BUILD_RPATH OFF)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH OFF)
set(CMAKE_INSTALL_RPATH "@executable_path/../lib")

# Папки вывода сборки
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# Исходники основного приложения
set(MAIN_SOURCES
    main.cpp
    src/file_utils/FilesUtils.cpp
    src/menu/Menu.cpp
)

# Алгоритмы в отдельных библиотеках
add_library(MagicSquareLib SHARED src/encryption/MagicSquareCipher.cpp)
add_library(HillLib         SHARED src/encryption/HillChiper.cpp)
add_library(ColumnarLib     SHARED src/encryption/ColumnarTransposition.cpp)

# Include-директория
target_include_directories(MagicSquareLib PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(HillLib         PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_include_directories(ColumnarLib     PUBLIC ${PROJECT_SOURCE_DIR}/include)

# Исполняемый файл
add_executable(cryptoapp ${MAIN_SOURCES})

# Линкуем все библиотеки
target_link_libraries(cryptoapp
    MagicSquareLib
    HillLib
    ColumnarLib
)

# Прописать RPATH в исполняемом файле
set_target_properties(cryptoapp PROPERTIES
    INSTALL_RPATH "@executable_path/../lib"
)

# ----------------------------------------
# Инструкции для установки
# ----------------------------------------

install(TARGETS cryptoapp
    RUNTIME DESTINATION bin
)

install(TARGETS MagicSquareLib HillLib ColumnarLib
    LIBRARY DESTINATION lib
)

install(FILES
    ${PROJECT_SOURCE_DIR}/README.md
    DESTINATION share/doc/cryptoapp
)
