#include "MainWindow.hpp"
#include "./ui_mainwindow.h"

#include <QSyntaxStyle>
#include <QCodeEditor>
#include <QGLSLCompleter>
#include <QGLSLHighlighter>

#define MIXIN_OPEN_MAIN_TAB(tool, title, widget)\
auto tabName = std::string(tool);\
    tabName.append(" - ");\
    tabName.append(title);\
    ui->tabWidget->addTab(widget, QString::fromStdString(tabName));

void testCodeEditor(MainWindow* thiss, Ui::MainWindow* ui)
{
    auto codeEditor = new QCodeEditor(thiss);
    MIXIN_OPEN_MAIN_TAB("Code Editor", "Ring.script", codeEditor);

    QFile fl(":/trinket.xml");

    if (!fl.open(QIODevice::ReadOnly))
    {
        return;
    }

    auto style = new QSyntaxStyle(thiss);
    if (!style->load(fl.readAll()))
    {
    }

    // Load the font from a file
    int fontId = QFontDatabase::addApplicationFont(":/fonts/JetBrainsMono[wght].ttf");
    if (fontId == -1) {
        qWarning("Failed to load font!");
    }

    // Get the family name of the loaded font
    QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
    if (fontFamilies.isEmpty()) {
        qWarning("No font families found!");
    }

    QString fontFamily = fontFamilies.first();
    QFont font(fontFamily, 11);

    codeEditor->setSyntaxStyle(style);
    codeEditor->setCompleter(new QGLSLCompleter(thiss));
    codeEditor->setHighlighter(new QGLSLHighlighter);

    codeEditor->setFont(font);
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    testCodeEditor(this, ui);
    // ui->tabWidget->addTab()
}

MainWindow::~MainWindow()
{
    delete ui;
}
