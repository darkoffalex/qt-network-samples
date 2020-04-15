#include <iostream>
#include <string>
#include <QTcpSocket>
#include <thread>
#include <mutex>
#include <QtPlugin>

/// Прослушиваемый порт
unsigned _port;
/// IP сервера
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

    try
    {
        // Ввод IP адреса
        std::cout << "Please enter IP: ";
        std::getline(std::cin, _ip);

        // Ввод прослушиваемого порта
        std::cout << "Please enter port: ";
        std::cin >> _port;
        std::cin.ignore();

        // Создание сокета и подключение к серверу
        QTcpSocket clientSocket;
        clientSocket.connectToHost(QString::fromStdString(_ip),_port);
        clientSocket.waitForConnected(-1);

        if(clientSocket.state() == QTcpSocket::ConnectedState){
            std::cout << "Connected to " << _ip << "(" << _port << ")" << std::endl;
        }else{
            throw std::runtime_error("Error: Can not establish connection to server.");
        }

        // Ожидать приветствия сервера
        char msg[255] = {};
        clientSocket.waitForReadyRead(-1);
        if(clientSocket.state() == QTcpSocket::ConnectedState){
            clientSocket.read(msg,255);
        }else{
            throw std::runtime_error("Error: Connection lost.");
        }

        // Вывод сообщения приветствия сервера
        std::cout << "Server: " << msg << std::endl;

        // Поток получения сообщений от сервера
        std::thread receiveThread([&](){
            bool connected = true;
            while(connected)
            {
                // Размер и буфер
                size_t msgSize = 0;
                char* msg = nullptr;

                // Получить длинну сообщения
                clientSocket.waitForReadyRead(-1);
                clientSocket.read(reinterpret_cast<char*>(&msgSize),sizeof(size_t));

                // Состояние подключения
                connected = clientSocket.state() == QTcpSocket::ConnectedState;

                // Если подключен
                if(connected){
                    // Выделить строчку под сообщение нужной длины и получить сообщение
                    msg = new char[msgSize];
                    // Читать из сокета покуда все не считается
                    size_t readTotal = 0;
                    do {
                        readTotal += clientSocket.read(msg, msgSize-readTotal);
                    } while (readTotal < msgSize);

                    std::cout << msg << std::endl;
                }

                // Удалить строчку
                delete[] msg;
            }
        });

        // Поток отправки сообщений серверу
        std::thread sendThread([&](){
            bool connected = true;
            while(connected)
            {
                // Состояние подключения
                connected = clientSocket.state() == QTcpSocket::ConnectedState;

                if(connected)
                {
                    // Ввод сообщения
                    std::string message;
                    std::getline(std::cin, message);

                    // Отправка длинны сообщения (добавляем единицу для завершающего нуля)
                    size_t size = message.size()+1;
                    clientSocket.write(reinterpret_cast<char*>(&size), sizeof(size_t));
                    // Отправка сообщения
                    clientSocket.write(message.c_str(), size);
                    clientSocket.waitForBytesWritten(-1);
                }
            }
        });

        receiveThread.join();
        sendThread.join();
    }
    catch(std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
        system("pause");
    }

    return 0;
}