#include "ChatWindow.h"
#include "./ui_ChatWindow.h"

#include <QMessageBox>
#include <QTcpSocket>

extern QTcpSocket* _socket;

/**
 * Конструктор окна
 * @param parent Родительский элемент
 */
ChatWindow::ChatWindow(QWidget *parent):QWidget(parent),ui_(new Ui::ChatWindow),spacerUnderMessages_(nullptr)
{
    // Инициализация UI
    ui_->setupUi(this);
    // Связать событие сокета и обработчик
    connect(_socket,SIGNAL(readyRead()),this,SLOT(readyReadMessage()));
}

/**
 * Деструктор окна
 */
ChatWindow::~ChatWindow()
{
    // Закрыть сокет соединения
    _socket->close();
    // Удалить UI
    delete ui_;
}

/**
 * Добавить сообщение на интерфейс
 * @param message Текст сообщения
 * @param my Свое же сообщение
 * @param author Автор сообщения
 */
void ChatWindow::addMessageToUi(const QString& message, bool my, const QString& author)
{
    // Если spacer установлен - удалить его
    if(spacerUnderMessages_ != nullptr){
        ui_->messagesVertialLayout->removeItem(spacerUnderMessages_);
    }else{
        spacerUnderMessages_ = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Expanding);
    }

    // Сформировать строку с сообщением
    QString resultMsg;
    if(!my && !author.isEmpty()) {
        resultMsg.append("<span style='font-weight:bold;'>").append(author).append("</span><br><br>");
    }
    resultMsg.append(message);

    // Создать виджет с сообщением и добавить его
    auto label = new QLabel(ui_->scrollArea);
    label->setText(resultMsg);
    label->setWordWrap(true);
    label->setProperty("my",my);
    label->setSizePolicy(QSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred));
    messages_.push_back(label);
    ui_->messagesVertialLayout->addWidget(label);

    // Добавить spacer в конец
    ui_->messagesVertialLayout->addSpacerItem(spacerUnderMessages_);
}

/// S L O T S

/**
 * Обработка события нажатия на кнопку отправки сообщения
 */
void ChatWindow::on_sendButton_clicked()
{
    // Строка с сообщением
    auto message = ui_->messageEditBox->toPlainText().toStdString();

    // Отправка длинны сообщения (добавляем единицу для завершающего нуля)
    size_t size = message.size()+1;
    _socket->write(reinterpret_cast<char*>(&size), sizeof(size_t));
    // Отправка сообщения
    _socket->write(message.c_str(), size);
    _socket->waitForBytesWritten(-1);

    // Добавить сообщение на UI
    this->addMessageToUi(QString::fromStdString(message), true);

    // Очистить поле ввода
    ui_->messageEditBox->setPlainText("");
}

/**
 * Обработка события готовности сокета к чтению данных
 */
void ChatWindow::readyReadMessage()
{
    // Получить отправителя события (сигнала) - сокет
    const auto socket = qobject_cast<QTcpSocket*>(sender());

    // Получить размер
    size_t msgSize;
    socket->read(reinterpret_cast<char*>(&msgSize),sizeof(size_t));

    // Выделить буфер
    char* msg = new char[msgSize];

    // Читать из сокета покуда все не считается
    size_t readTotal = 0;
    do {
        readTotal += socket->read(msg, msgSize-readTotal);
    } while (readTotal < msgSize);

    // Получить автора и сообщение отдельно
    auto msgString = QString(msg);
    auto msgParts = msgString.split(':');
    auto author = msgParts[0];
    auto message = msgString.replace(author + ":","");

    // Добавить сообщение в UI
    this->addMessageToUi(message, false,author);
}
