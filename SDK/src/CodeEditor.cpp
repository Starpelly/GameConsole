#include "CodeEditor.hpp"
#include "ui_codeeditor.h"

#include <QSyntaxStyle>
#include <QCodeEditor>
#include <QLuaCompleter>
#include <QLuaHighlighter>

#include <QTextStream>

QString readFileToString(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString(); // or handle the error as needed
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");
    QString content = in.readAll();
    file.close();
    return content;
}

CodeEditor::CodeEditor(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CodeEditor)
{
    ui->setupUi(this);

    // Create editor
    editor = new QCodeEditor(this);
    ui->verticalLayout->addWidget(editor);

    QFile fl(":/trinket.xml");

    if (!fl.open(QIODevice::ReadOnly))
    {
        return;
    }

    auto style = new QSyntaxStyle(this);
    if (!style->load(fl.readAll()))
    {
    }

    // Load the font from a file
    int fontId = QFontDatabase::addApplicationFont(":/fonts/Consolas.ttf");
    if (fontId == -1)
    {
        qWarning("Failed to load font!");
    }

    // Get the family name of the loaded font
    QStringList fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
    if (fontFamilies.isEmpty())
    {
        qWarning("No font families found!");
    }

    QString fontFamily = fontFamilies.first();
    QFont font(fontFamily, 11);

    editor->setSyntaxStyle(style);
    editor->setCompleter(new QLuaCompleter(this));
    editor->setHighlighter(new QLuaHighlighter);

    editor->setFont(font);
}

CodeEditor::~CodeEditor()
{
    delete ui;
}

void CodeEditor::OpenFile(const QString &filePath)
{
    editor->setText(readFileToString(filePath));
}
