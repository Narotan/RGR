cmake_minimum_required(VERSION 3.10)
project(CryptoApp LANGUAGES CXX VERSION 1.0.0)

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

# Установка
install(TARGETS cryptoapp
    RUNTIME DESTINATION bin
)

install(TARGETS MagicSquareLib
    LIBRARY DESTINATION lib
)

# Документация
install(FILES README.md LICENSE DESTINATION share/doc/cryptoapp)

# Настройка CPack
include(InstallRequiredSystemLibraries)
set(CPACK_PACKAGE_NAME "CryptoApp")
set(CPACK_PACKAGE_VENDOR "YourCompany")
set(CPACK_PACKAGE_DESCRIPTION "Advanced encryption toolkit")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/README.md")
set(CPACK_STRIP_FILES TRUE)

# Настройки для macOS
if(APPLE)
    set(CPACK_GENERATOR "TGZ")  # Только tar.gz для простоты
endif()

# Настройки для Linux
if(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "TGZ")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Your Name <your@email.com>")
    set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
endif()

include(CPack)