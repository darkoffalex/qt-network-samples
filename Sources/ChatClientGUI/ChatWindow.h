#pragma once

#include <QWidget>
#include <QSpacerItem>
#include <QLabel>

// Класс для взаимодействия с UI интерфейсом
QT_BEGIN_NAMESPACE
namespace Ui { class ChatWindow; }
QT_END_NAMESPACE

class ChatWindow final : public QWidget
{
Q_OBJECT
public:
    /**
     * Конструктор окна
     * @param parent Родительский элемент
     */
    explicit ChatWindow(QWidget *parent = nullptr);

    /**
     * Деструктор окна
     */
    ~ChatWindow() override;

    /**
     * Добавить сообщение на интерфейс
     * @param message Текст сообщения
     * @param my Свое же сообщение
     * @param author Автор сообщения
     */
    void addMessageToUi(const QString& message, bool my = false, const QString& author = "");

public slots:
    /**
     * Обработка события готовности сокета к чтению данных
     */
    void readyReadMessage();

private slots:

    /**
     * Обработка события нажатия на кнопку отправки сообщения
     */
    void on_sendButton_clicked();

private:
    /// Указатель на UI объект
    Ui::ChatWindow *ui_;

    /// Указатель на spacer под сообщениями
    QSpacerItem* spacerUnderMessages_;

    /// Список всех сообщений
    QList<QLabel*> messages_;
};
