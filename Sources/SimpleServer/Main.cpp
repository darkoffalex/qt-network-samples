#include <iostream>
#include <QTcpServer>
#include <QTcpSocket>
#include <QtPlugin>

/// Прослушиваемый порт
unsigned _port;

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

    // Ввод прослушиваемого порта
    std::cout << "Please enter port: ";
    std::cin >> _port;

    // Инициализация TCP сервера
    QTcpServer tcpServer;
    tcpServer.listen(QHostAddress::Any, static_cast<quint16>(_port));

    // Ожидать новых подключений
    std::cout << "Listening port (" << _port << ")" << std::endl;
    tcpServer.waitForNewConnection(-1);

    // Получить сокет подключивашегося клиента
    auto socketClientCon = tcpServer.nextPendingConnection();

    // Если подключение состоялось
    if(socketClientCon->state() == QTcpSocket::SocketState::ConnectedState){
        // Информация о клиенте
        std::cout << "Client connected (" << socketClientCon->peerAddress().toString().toStdString() << ")" << std::endl;

        // Отправка приветствия клиенту
        char strSalutation[255] = "You successfully connected!";
        socketClientCon->write(strSalutation,255);
        socketClientCon->waitForBytesWritten(-1);
        std::cout << "Salutation sent" << std::endl;
    }else{
        std::cout << "Error: Can not establish connection." << std::endl;
    }

    system("pause");
    return 0;
}