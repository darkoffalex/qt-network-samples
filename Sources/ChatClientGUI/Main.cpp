#include "ConnectionWindow.h"

#include <windows.h>
#include <QtPlugin>
#include <QApplication>
#include <QFontDatabase>
#include <QTcpSocket>
#include <QIcon>

/// Сокет клиента
QTcpSocket* _socket = nullptr;

/**
 * Точка входа
 * @param hInstance Дескриптор модуля Windows
 * @param hPrevInstance Не используется (устарело)
 * @param pCmdLine Аргументы запуска
 * @param nCmdShow Вид показа окна
 * @return Код выполнения
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    // Если QT линкуется статически, нужно статически импортировать плагин
#ifdef QT_STATIC_BUILD
    Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#endif

    // Инициализация QT
    int argc = 0;
    char* argv[] = {{}};
    QApplication app(argc, argv);

    // Добавить шрифт из ресурсов
    QFontDatabase::addApplicationFont(":/fonts/ProximaNova-Regular.otf");
    QFontDatabase::addApplicationFont(":/fonts/ProximaNova-Semibold.ttf");

    // Окно с диалогом подключения к серверу
    ConnectionWindow connectionWindow;
    connectionWindow.setWindowIcon(QIcon(":/icons/icon.png"));
    connectionWindow.show();

    // Исполнение приложения
    return QApplication::exec();
}