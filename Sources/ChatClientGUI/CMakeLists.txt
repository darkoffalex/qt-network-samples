# Версия CMake
cmake_minimum_required(VERSION 3.5)

# Определить разрядность платформы
if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
    set(PLATFORM_BIT_SUFFIX "x86")
else()
    set(PLATFORM_BIT_SUFFIX "x64")
endif()

# Для того чтобы была возможность включать файлы созданные генератором QT расположенные в CMAKE промежуточном каталоге
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Название цели сборки
set(TARGET_NAME "ChatClientGUI")
set(TARGET_BIN_NAME "ChatClientGUI")

# Пути к библиотеке QT для различных компиляторов и платформ
include("../../QtDir.cmake")

# Поиск пакета QT средствами CMAKE
set(CMAKE_PREFIX_PATH "${QT5_DIR}")
find_package(Qt5 COMPONENTS Widgets Network REQUIRED)

# Включение генераторов QT
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Добавляем .exe (проект в Visual Studio)
add_executable(${TARGET_NAME} WIN32
        "Main.cpp"
        "ConnectionWindow.h" "ConnectionWindow.cpp" "ConnectionWindow.ui"
        "ChatWindow.h" "ChatWindow.cpp" "ChatWindow.ui"

        "../../Resources/ChatClientGUI/resources.qrc"
        "../../Resources/ChatClientGUI/version.rc")

# Меняем название запускаемого файла в зависимости от типа сборки
set_property(TARGET ${TARGET_NAME} PROPERTY OUTPUT_NAME "${TARGET_BIN_NAME}$<$<CONFIG:Debug>:_Debug>_${PLATFORM_BIT_SUFFIX}")

# Если это статическая линковка - объявить символ QT_STATIC_BUILD (может понадобиться в исходном коде)
if(QT_STATIC_LINK)
    target_compile_definitions(${TARGET_NAME} PUBLIC QT_STATIC_BUILD)
endif()


# Дополнительные действия
if(WIN32)
    # Если это не статическая линковка - необходимо скопировать DLL в папку с исполняемым файлом
    if(NOT QT_STATIC_LINK)
        ADD_CUSTOM_COMMAND(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy ${QT5_DIR}bin\\Qt5Core$<$<CONFIG:Debug>:d>.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
                COMMAND ${CMAKE_COMMAND} -E copy ${QT5_DIR}bin\\Qt5Widgets$<$<CONFIG:Debug>:d>.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
                COMMAND ${CMAKE_COMMAND} -E copy ${QT5_DIR}bin\\Qt5Gui$<$<CONFIG:Debug>:d>.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
                COMMAND ${CMAKE_COMMAND} -E copy ${QT5_DIR}bin\\Qt5Network$<$<CONFIG:Debug>:d>.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
                COMMENT "Copying Qt binaries" VERBATIM)
        # Если это GNU (MinGW) нужны еще runtime-библиотеки
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            ADD_CUSTOM_COMMAND(TARGET ${TARGET_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy ${QT5_DIR}bin\\libgcc_s_dw2-1.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
#                    COMMAND ${CMAKE_COMMAND} -E copy ${QT5_DIR}bin\\libstdc++-6.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
                    COMMAND ${CMAKE_COMMAND} -E copy ${QT5_DIR}bin\\libwinpthread-1.dll ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
                    COMMENT "Copying MinGW runtime binaries for Qt" VERBATIM)
        endif()
    endif()
else()
    # TODO: дополнительные действия для других ОС
endif()

# Дополнительные библиотеки для линковки с приложением
SET(ADDITIONAL_LIBS "")

# Если QT линкуется статически
if(QT_STATIC_LINK)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        SET(LIB_EXT ".lib")
    else()
        SET(LIB_EXT ".a")
    endif()

    # Основные модули QT
    SET(ADDITIONAL_LIBS ${ADDITIONAL_LIBS}
            "${QT5_DIR}/lib/qtpcre2$<$<CONFIG:Debug>:d>${LIB_EXT}"
            "${QT5_DIR}/lib/qtlibpng$<$<CONFIG:Debug>:d>${LIB_EXT}"
            "${QT5_DIR}/lib/qtharfbuzz$<$<CONFIG:Debug>:d>${LIB_EXT}"
            "${QT5_DIR}/lib/qtfreetype$<$<CONFIG:Debug>:d>${LIB_EXT}"
            "${QT5_DIR}/lib/Qt5FontDatabaseSupport$<$<CONFIG:Debug>:d>${LIB_EXT}"
            "${QT5_DIR}/lib/Qt5ThemeSupport$<$<CONFIG:Debug>:d>${LIB_EXT}"
            "${QT5_DIR}/lib/Qt5EventDispatcherSupport$<$<CONFIG:Debug>:d>${LIB_EXT}"
            "${QT5_DIR}/lib/Qt5AccessibilitySupport$<$<CONFIG:Debug>:d>${LIB_EXT}")

    if(WIN32)
        # Плагины
        SET(ADDITIONAL_LIBS ${ADDITIONAL_LIBS} "${QT5_DIR}/plugins/platforms/qwindows$<$<CONFIG:Debug>:d>${LIB_EXT}")
        # Библиотеки ОС
        SET(ADDITIONAL_LIBS ${ADDITIONAL_LIBS} "imm32.lib" "version.lib" "netapi32.lib" "uxtheme.lib" "dwmapi.lib" "Ws2_32.lib" "Winmm.lib" "Iphlpapi.lib")
    endif()
endif()

# Линковка приложения и дополнительных библиотек
target_link_libraries(${TARGET_NAME} "Qt5::Widgets" "Qt5::Network" ${ADDITIONAL_LIBS})