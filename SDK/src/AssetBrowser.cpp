#include "AssetBrowser.hpp"

#include <QApplication>

QTreeWidgetItem* scriptsFolder;

AssetBrowser::AssetBrowser(QWidget* parent) : QTreeWidget{parent}
{
    // Config
    {
        this->setContextMenuPolicy(Qt::CustomContextMenu);
        this->setHeaderHidden(true);
    }

    CreateFolder("Palettes"); 
    scriptsFolder = CreateFolder("Scripts");
    CreateFolder("SoundFX");
    CreateFolder("Sprites");

    PopulateTree();
}

QTreeWidgetItem *AssetBrowser::CreateFolder(const QString &name)
{
    auto folder = new QTreeWidgetItem(this);
    folder->setText(0, name);
    folder->setSizeHint(0, QSize(100, 20));
    folder->setIcon(0, qApp->style()->standardIcon(QStyle::SP_DirIcon));

    return folder;
}

void AssetBrowser::PopulateTree()
{
    const auto item = new QTreeWidgetItem(scriptsFolder);
    item->setText(0, QString::fromStdString("Game.lua"));
    item->setIcon(0, qApp->style()->standardIcon(QStyle::SP_FileIcon));
    // item->setData(0, Qt::UserRole, QVariant::fromValue((uint64_t)sprite.first));
}
