#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    auto OpenTab(const QString& toolName, const QString& title, QWidget* widget) -> void;
    auto OpenTab(const QString& toolName, const QString& title, QWidget* widget, const QIcon& icon) -> void;

    auto OpenCodeEditor(const QString& path) -> void;

    auto OpenPlaytest() -> void;
    auto OnPlaytestClose() -> void;
    auto SetToolsEnabled(bool val) -> void;
private:
    void onCloseTab(int index);

private:
    Ui::MainWindow *ui;
};

MainWindow* GetMainWindow();

#endif // MAINWINDOW_HPP
