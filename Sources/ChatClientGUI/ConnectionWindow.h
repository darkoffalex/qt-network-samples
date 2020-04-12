#pragma once

#include "ChatWindow.h"
#include <QWidget>

// Класс для взаимодействия с UI интерфейсом
QT_BEGIN_NAMESPACE
namespace Ui { class ConnectionWindow; }
QT_END_NAMESPACE

class ConnectionWindow final : public QWidget
{
Q_OBJECT

public:
    /**
     * Конструктор окна
     * @param parent Родительский элемент
     */
    explicit ConnectionWindow(QWidget *parent = nullptr);

    /**
     * Деструктор окна
     */
    ~ConnectionWindow() override;

private slots:
    /**
     * Обработка события нажатия на кнопку подключения
     */
    void on_connectButton_clicked();

private:
    /// Указатель на UI объект
    Ui::ConnectionWindow *ui_;
    /// Окно чата
    ChatWindow* chatWindow_;
};