#include "MainWindow.hpp"
#include "./ui_mainwindow.h"

#include <QDir>

#include "AssetBrowser.hpp"
#include "SDK/Tools/CodeEditor.hpp"
#include "SDK/Widgets/GameWindow.hpp"

#include "SDK.hpp"

static MainWindow* g_MainWindow = nullptr;
MainWindow* GetMainWindow()
{
    return g_MainWindow;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    g_MainWindow = this;

    ui->setupUi(this);

    // Asset browser
    {
        auto* assetBrowser = new AssetBrowser();
        assetBrowser->setMaximumWidth(250);
        ui->assetTreeLayout->addWidget(assetBrowser);
    }

    // Tab setup
    {
        ui->tabWidget->setTabsClosable(true);
        connect(ui->tabWidget, &QTabWidget::tabCloseRequested, this, &MainWindow::onCloseTab);
    }

    // Actions
    {
        connect(ui->actionPlayGame, &QAction::triggered, this, [this]() {
            this->OpenPlaytest();
        });
    }

    OpenCodeEditor(SDK::GetProjectDataPath() + "Scripts/Test.lua");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OpenTab(const QString& toolName, const QString& title, QWidget* widget)
{
    auto tabName = toolName;
    tabName.append(" - ");
    tabName.append(title);
    ui->tabWidget->addTab(widget, tabName);
}

void MainWindow::OpenTab(const QString &toolName, const QString &title, QWidget *widget, const QIcon &icon)
{
    auto tabName = toolName;
    tabName.append(" - ");
    tabName.append(title);
    ui->tabWidget->addTab(widget, icon, tabName);
}

void MainWindow::OpenCodeEditor(const QString &path)
{
    auto editor = new CodeEditor();
    editor->OpenFile(path);

    QString relativePath = QDir(SDK::GetProjectDataPath()).relativeFilePath(path);
    OpenTab("Code Editor", relativePath, editor, QIcon(":/icons/lua.svg"));
}

void MainWindow::OpenPlaytest()
{
    auto* gameWindow = new GameWindow(this);
    gameWindow->show();
    connect(gameWindow, &GameWindow::whenClosed, this, [this, gameWindow]()
    {
        delete gameWindow;
        OnPlaytestClose();
    });

    SetToolsEnabled(false);
}

void MainWindow::OnPlaytestClose()
{
    SetToolsEnabled(true);
}

void MainWindow::SetToolsEnabled(bool val)
{
    ui->actionPlayGame->setEnabled(val);
    ui->actionPlayScene->setEnabled(val);
    ui->actionBuildGame->setEnabled(val);
    ui->actionExportGame->setEnabled(val);
    ui->actionGameSettings->setEnabled(val);
}

void MainWindow::onCloseTab(int index)
{
    auto widget = ui->tabWidget->widget(index);
    ui->tabWidget->removeTab(index);
    delete widget;
}
