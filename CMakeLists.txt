# Версия CMake
cmake_minimum_required(VERSION 3.5)

# Название проекта (решение в Visual Studio)
project(QtNetworkSamples)

# Стандарт С/С++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Устанавливаем каталоги для выходных файлов
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/Bin)

# Установка линковки для QT библиотеки
# Линковку необходимо установить глобально, в противном случае у CMake будет какая-то хуйбалистика с путями (CMAKE_PREFIX_PATH)
set(QT_STATIC_LINK ON)

# П Р О С Т Е Й Ш И Й  П Р И М Е Р

# Сервер
add_subdirectory("Sources/SimpleServer")

# Клиент
add_subdirectory("Sources/SimpleClient")

# Ч А Т

# Сервер
add_subdirectory("Sources/ChatServerMT")

# Клиент
add_subdirectory("Sources/ChatClient")

# Клиент
add_subdirectory("Sources/ChatClientGUI")
