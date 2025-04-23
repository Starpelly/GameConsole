#ifndef GAMEWINDOW_HPP
#define GAMEWINDOW_HPP

#include <QMainWindow>
#include <QTimer>

namespace Ui {
class GameWindow;
}

class GameWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GameWindow(QWidget *parent = nullptr);
    ~GameWindow();

protected:
    void showEvent(QShowEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

    Q_SLOT void doOneFrame();

signals:
    void whenClosed();

private:
    Ui::GameWindow *ui;

    QTimer timer;
};

#endif // GAMEWINDOW_HPP
