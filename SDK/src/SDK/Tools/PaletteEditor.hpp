#ifndef PALETTEEDITOR_HPP
#define PALETTEEDITOR_HPP

#include <QWidget>

namespace Ui {
class PaletteEditor;
}

class PaletteEditor : public QWidget
{
    Q_OBJECT

public:
    explicit PaletteEditor(QWidget *parent = nullptr);
    ~PaletteEditor();

private:
    Ui::PaletteEditor *ui;
};

#endif // PALETTEEDITOR_HPP
