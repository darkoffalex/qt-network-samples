#include <iostream>
#include <string>
#include <QTcpSocket>
#include <QtPlugin>

/// Порт целевого сервера
unsigned _port;
/// IP целевого сервера
std::string _ip;

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

    // Ввод IP'а целевого сервера
    std::cout << "Please enter IP: ";
    std::getline(std::cin, _ip);

    // Ввод прослушиваемого порта
    std::cout << "Please enter port: ";
    std::cin >> _port;

    // Создание сокета подключения
    QTcpSocket clientSocket;
    clientSocket.connectToHost(QString::fromStdString(_ip),_port);
    clientSocket.waitForConnected(-1);

    if(clientSocket.state() == QTcpSocket::ConnectedState){
        std::cout << "Connected to " << _ip << "(" << _port << ")" << std::endl;
    }else{
        std::cout << "Error: Can not establish connection to server." << std::endl;
    }

    // Ожидать считывания данных отправленных сервером
    char msg[255] = {};
    clientSocket.waitForReadyRead(-1);
    clientSocket.read(msg,255);

    // Вывод сообщения сервера
    std::cout << "Server: " << msg << std::endl;

    system("pause");
    return 0;
}