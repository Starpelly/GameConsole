#ifndef PALETTEEDITOR_HPP
#define PALETTEEDITOR_HPP

#include <QWidget>

#include "Engine/Core/Engine.hpp"

struct PaletteColor
{
    PaletteColor() {}
    PaletteColor(uint8 r, uint8 g, uint8 b)
    {
        this->r = r;
        this->g = g;
        this->b = b;
    }

    PaletteColor(QColor c)
    {
        this->r = c.red();
        this->g = c.green();
        this->b = c.blue();
    }

    inline QColor toQColor() const { return QColor(r, g, b); }

    uint8 r = 0x00;
    uint8 g = 0x00;
    uint8 b = 0x00;
};

namespace Ui {
class PaletteEditor;
}

class PaletteWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PaletteWidget(QWidget* parent = nullptr);

    QColor color = 0xFF00FF;
    QList<PaletteColor>* palette;

protected:
    void paintEvent(QPaintEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void leaveEvent(QEvent*) override;

private:
    short highlight = -1;
    short selection = -1;

    short lastHighlight = -1;
};

class PaletteEditor : public QWidget
{
    Q_OBJECT

public:
    explicit PaletteEditor(const QString& path, QWidget *parent = nullptr);
    ~PaletteEditor();

    QList<PaletteColor> palette;

    void Init(const QString& path);

private:
    PaletteWidget* widget;

private:
    Ui::PaletteEditor *ui;
};

#endif // PALETTEEDITOR_HPP
