#include "PaletteEditor.hpp"
#include "ui_paletteeditor.h"

PaletteEditor::PaletteEditor(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PaletteEditor)
{
    ui->setupUi(this);
}

PaletteEditor::~PaletteEditor()
{
    delete ui;
}
