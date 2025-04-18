#include "AssetBrowser.hpp"

#include <QApplication>

AssetBrowser::AssetBrowser(QWidget* parent) : QTreeWidget{parent}
{
    // Config
    {
        this->setContextMenuPolicy(Qt::CustomContextMenu);
        this->setHeaderHidden(true);
    }

    CreateFolder("Game");
    CreateFolder("Images");
    CreateFolder("Music");
    CreateFolder("Objects");
    CreateFolder("Palettes"); 
    CreateFolder("SoundFX");
    CreateFolder("Scenes");
    CreateFolder("Sprites");
    CreateFolder("Strings");
    CreateFolder("Video");
}

QTreeWidgetItem *AssetBrowser::CreateFolder(const QString &name)
{
    auto folder = new QTreeWidgetItem(this);
    folder->setText(0, name);
    folder->setSizeHint(0, QSize(100, 20));
    folder->setIcon(0, qApp->style()->standardIcon(QStyle::SP_DirIcon));

    return folder;
}
