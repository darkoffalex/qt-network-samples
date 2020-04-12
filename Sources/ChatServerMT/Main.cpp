#include <iostream>
#include <QTcpServer>
#include <QTcpSocket>
#include <thread>
#include <mutex>
#include <QtPlugin>

/// Прослушиваемый порт
unsigned _port;
/// Состояние сервера
bool _serverOn = true;
/// Список клиентских сокетов
QList<QTcpSocket*> _clientSockets;
/// Мьютекс для массива сокетов (массив меняется из нескольких потоков)
std::mutex _socketLstMtx;

/**
 * Функция клиента (получение сообщений от конкретного клиента и рассылка его остальным)
 * @param clientSocket Идентификатор клиента (индекс в массиве сокетов)
 */
void clientThreadFunction(QTcpSocket* clientSocket);

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
    std::cout << "Listening port (" << _port << ")" << std::endl;

    // Основной цикл сервера
    while(_serverOn)
    {
        // Ожидать новых подключений
        tcpServer.waitForNewConnection(-1);

        // Получить сокет подключивашегося клиента
        QTcpSocket* socketClientCon = tcpServer.nextPendingConnection();

        // Если соединение установлено
        if(socketClientCon->state() == QTcpSocket::ConnectedState)
        {
            // Информация о клиенте
            std::cout << "Client " << socketClientCon << " connected (" << socketClientCon->peerAddress().toString().toStdString() << ")" << std::endl;

            // Отправка приветствия клиенту
            char strSalutation[255] = "You successfully connected!";
            socketClientCon->write(strSalutation,255);
            socketClientCon->waitForBytesWritten(-1);

            // Добавить сокет соединения в массив
            _socketLstMtx.lock();
            _clientSockets.push_back(socketClientCon);
            _socketLstMtx.unlock();

            // Запуск потока клиента
            std::thread clientThread(clientThreadFunction, socketClientCon);
            clientThread.detach();
        }
        else{
            std::cout << "Error: Can not establish connection with client." << std::endl;
        }
    }

    system("pause");
    return 0;
}

/**
 * Функция клиента (получение сообщений от конкретного клиента и рассылка его остальным)
 * @param clientSocket Идентификатор клиента (индекс в массиве сокетов)
 */
void clientThreadFunction(QTcpSocket* clientSocket)
{
    bool connected = true;
    while(connected)
    {
        // Получить длинну сообщения
        size_t msgSize = 0;
        clientSocket->waitForReadyRead(-1);
        clientSocket->read(reinterpret_cast<char*>(&msgSize),sizeof(size_t));

        // Выделить строчку под сообщение нужной длины и получить сообщение
        char* msg = new char[msgSize];
        clientSocket->waitForReadyRead(10);
        clientSocket->read(msg,msgSize);

        // Состояние подключения
        connected = clientSocket->state() == QTcpSocket::ConnectedState;

        // После получения сообщения - отправить его всем клиентам кроме текущего
        _socketLstMtx.lock();
        for(auto socket : _clientSockets)
        {
            // Номер клиента
            char clientIdStr[10];
            _itoa_s(reinterpret_cast<uintptr_t>(clientSocket), clientIdStr, sizeof(clientIdStr));

            // Сообщение для отправки
            std::string msgToSend;

            if(connected){
                msgToSend = std::string("Client #").append(clientIdStr).append(": ").append(msg);
            }else{
                msgToSend = std::string("Client #").append(clientIdStr).append(" left the chat");
            }

            // Если клиент не тот, что отправил сообщение
            if(socket != clientSocket) {
                // Отправка длинны сообщения (добавляем единицу для завершающего нуля)
                size_t size = msgToSend.size()+1;
                socket->write(reinterpret_cast<char*>(&size), sizeof(size_t));
                socket->waitForBytesWritten(-1);
                // Отправка сообщения
                socket->write(msgToSend.c_str(), size);
                socket->waitForBytesWritten(-1);
            }
        }
        _socketLstMtx.unlock();

        // Удалить строку
        delete[] msg;
    }

    // Сообщение об отключении клиента
    std::cout << "Client " << clientSocket << " disconnected" << std::endl;

    // Удалить из массива дескриптор сокета
    _socketLstMtx.lock();
    _clientSockets.removeOne(clientSocket);
    _socketLstMtx.unlock();
}