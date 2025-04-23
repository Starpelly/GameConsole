#ifndef CODEEDITOR_HPP
#define CODEEDITOR_HPP

#include <QWidget>

namespace Ui {
class CodeEditor;
}

class QCodeEditor;

class CodeEditor : public QWidget
{
    Q_OBJECT

public:
    explicit CodeEditor(QWidget *parent = nullptr);
    ~CodeEditor();

    auto OpenFile(const QString& filePath) -> void;
    auto SaveFile() -> void;

private:
    Ui::CodeEditor *ui;
    QCodeEditor* editor;
    QString filePath;
};

#endif // CODEEDITOR_HPP
