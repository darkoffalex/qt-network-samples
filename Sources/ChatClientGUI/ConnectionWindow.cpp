#include "ConnectionWindow.h"
#include "./ui_ConnectionWindow.h"

#include <QTcpSocket>
#include <QMessageBox>

extern QTcpSocket* _socket;

/**
 * Конструктор окна
 * @param parent Родительский элемент
 */
ConnectionWindow::ConnectionWindow(QWidget *parent):QWidget(parent),ui_(new Ui::ConnectionWindow),chatWindow_(nullptr)
{
    // Инициализация UI
    ui_->setupUi(this);
}

/**
 * Деструктор окна
 */
ConnectionWindow::~ConnectionWindow()
{
    delete ui_;
    delete chatWindow_;
}

/**
 * Обработка события нажатия на кнопку подключения
 */
void ConnectionWindow::on_connectButton_clicked()
{
    // Создать сокет и подключиться к серверу
    _socket = new QTcpSocket;
    _socket->connectToHost(ui_->ipEdit->text(),ui_->portEdit->text().toUInt());
    _socket->waitForConnected(1000);

    // Если подключение успешно
    if(_socket->state() == QTcpSocket::ConnectedState)
    {
        // Считать приветствие сервера
        char msg[255] = {};
        _socket->waitForReadyRead(-1);
        _socket->read(msg,255);

        // Закрыть текущее окно и открыть окно чата
        this->close();
        chatWindow_ = new ChatWindow();
        chatWindow_->setWindowIcon(this->windowIcon());
        chatWindow_->show();
    }
    else
    {
        QMessageBox msgBox;
        msgBox.setText("Can't connect to server");
        msgBox.setWindowTitle("Error");
        msgBox.setIcon(QMessageBox::Icon::Critical);
        msgBox.exec();
    }
}
