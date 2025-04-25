#include "MainWindow.hpp"
#include "./ui_mainwindow.h"

#include <QDir>

#include "AssetBrowser.hpp"

// Tools
#include "SDK/Tools/CodeEditor.hpp"
#include "SDK/Tools/PaletteEditor.hpp"
#include "SDK/Tools/MusicEditor.hpp"

// Widgets
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

    setWindowTitle("Soulcast Software Development Kit - " + SDK::GetProjectPath());

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

    // OpenCodeEditor(SDK::GetProjectDataPath() + "Scripts/Test.lua");
    // OpenPaletteEditor(QUuid(), SDK::GetProjectDataPath() + "Palettes/mario.pal");
    OpenMusicEditor(QUuid(), SDK::GetProjectDataPath() + "Music/smw.mid");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OpenTab(const QUuid& uuid, const QString& toolName, const QString& title, QWidget* widget)
{
    auto tabName = toolName;
    tabName.append(" - ");
    tabName.append(title);
    auto index = ui->tabWidget->addTab(widget, tabName);
    ui->tabWidget->setCurrentIndex(index);
}

void MainWindow::OpenTab(const QUuid& uuid, const QString &toolName, const QString &title, QWidget *widget, const QIcon &icon)
{
    if (openTabs.contains(uuid))
    {
        QWidget* widget = openTabs[uuid];
        int index = ui->tabWidget->indexOf(widget);
        if (index != -1)
        {
            ui->tabWidget->setCurrentIndex(index);
            return;
        }
        else
        {
            openTabs.remove(uuid); // Clean up stale entry just in case
        }
    }

    auto tabName = toolName;
    tabName.append(" - ");
    tabName.append(title);
    auto index = ui->tabWidget->addTab(widget, icon, tabName);
    ui->tabWidget->setCurrentIndex(index);
    openTabs[uuid] = widget;
}

void MainWindow::onCloseTab(int index)
{
    auto* closedWidget = ui->tabWidget->widget(index);

    // Remove from UUID map
    auto it = std::find_if(openTabs.begin(), openTabs.end(),
        [closedWidget](QWidget* widget) {
            return widget == closedWidget;
        });

    if (it != openTabs.end())
        openTabs.erase(it);

    ui->tabWidget->removeTab(index);
    delete closedWidget;
}

void MainWindow::OpenCodeEditor(const QUuid& uuid, const QString &path)
{
    auto editor = new CodeEditor();
    editor->OpenFile(path);

    QString relativePath = QDir(SDK::GetProjectDataPath()).relativeFilePath(path);
    OpenTab(uuid, "Code Editor", relativePath, editor, QIcon(":/icons/lua.svg"));
}

void MainWindow::OpenPaletteEditor(const QUuid &uuid, const QString &path)
{
    auto editor = new PaletteEditor(path);
    // editor->OpenFile(path);

    QString relativePath = QDir(SDK::GetProjectDataPath()).relativeFilePath(path);
    OpenTab(uuid, "Palette Editor", relativePath, editor, QIcon(":/icons/sfc.svg"));
}

void MainWindow::OpenMusicEditor(const QUuid &uuid, const QString &path)
{
    auto editor = new MusicEditor(path);

    QString relativePath = QDir(SDK::GetProjectDataPath()).relativeFilePath(path);
    OpenTab(uuid, "Music Editor", relativePath, editor, QIcon(":/icons/midi.png"));
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
