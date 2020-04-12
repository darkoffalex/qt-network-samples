#include "ConnectionWindow.h"

#include <QtPlugin>
#include <QApplication>
#include <QFontDatabase>
#include <QTcpSocket>
#include <QIcon>

/// Сокет клиента
QTcpSocket* _socket = nullptr;

/**
 * Точка входа
 * @param argc Кол-во аргументов
 * @param argv Аргументы
 * @return Код выхода
 */
int main(int argc, char* argv[])
{
    // Если QT линкуется статически, нужно статически импортировать плагин
#ifdef QT_STATIC_BUILD
    Q_IMPORT_PLUGIN(QWindowsIntegrationPlugin)
#endif

    // Инициализация QT
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