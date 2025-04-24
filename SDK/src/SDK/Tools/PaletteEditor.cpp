#include "PaletteEditor.hpp"
#include "ui_paletteeditor.h"

#include <QPainter>
#include <QMouseEvent>
#include <QEvent>
#include <QToolTip>

PaletteEditor::PaletteEditor(const QString& path, QWidget *parent)
    : QWidget(parent)
    , widget(new PaletteWidget(this))
    , ui(new Ui::PaletteEditor)
{
    ui->setupUi(this);

    Init(path);
}

PaletteEditor::~PaletteEditor()
{
    delete ui;
}

void PaletteEditor::Init(const QString& path)
{
    widget->palette = &palette;

    ui->widgetLayout->addWidget(widget, 1);

    auto test = Soulcast::Palette::LoadJASCPalette(path.toStdString());
    for (int i = 0; i < PALETTE_BANK_SIZE; i++)
    {
        if (i >= test.size())
            palette.push_back(PaletteColor());
        else
            palette.push_back(PaletteColor(test[i].r, test[i].g, test[i].b));
    }
}

// -------------
// PaletteWidget
// -------------
PaletteWidget::PaletteWidget(QWidget* parent) : QWidget(parent)
{
    setMouseTracking(true);
}

void PaletteWidget::paintEvent(QPaintEvent* )
{
    QPainter p(this);
    auto* editor = (PaletteEditor*)parent();

    QRectF rect(0, 0, (qreal)width() / 16, (qreal)height() / 16);
    short index = -1;
    if (!palette)
        return;

    for (uint8 y = 0; y < palette->count() / 0x10; ++y)
    {
        for (uint8 x = 0; x < 0x10; ++x)
        {
            ++index;
            if (index >= palette->count())
                return;

            QPen pen(qApp->palette().base(), 2);
            p.setPen(pen);
            PaletteColor clr = palette->at(index);
            p.setBrush(clr.toQColor());

            p.drawRect(rect.translated(x * (qreal)width() / 16, y * (qreal)height() / 16));

            if (highlight == index)
            {
                p.setBrush(qApp->palette().highlight());
                p.setPen(Qt::NoPen);
                p.setOpacity(0.7);
                p.drawRect(rect.translated(x * (qreal)width() / 16, y * (qreal)height() / 16));
                p.setOpacity(1.0);
            }
        }
    }
}

void PaletteWidget::mouseMoveEvent(QMouseEvent* event)
{
    uint8 x = floor((float)(event->x() / ((qreal)width() / 16)));
    uint8 y = floor((float)(event->y() / ((qreal)height() / 16)));

    if (y > 15)
        y = 15;

    highlight = x % 16 + y * 16;

    if (highlight != lastHighlight)
    {
        // QToolTip::showText(event->globalPos(), "Hello!!!");
    }

    lastHighlight = highlight;

    update();
}

void PaletteWidget::leaveEvent(QEvent *)
{

}
