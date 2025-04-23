#include "AssetBrowser.hpp"

#include <QApplication>
#include <QDir>
#include <QDirIterator>
#include <QDebug>

#include "MainWindow.hpp"
#include "SDK.hpp"

QTreeWidgetItem* scriptsFolder;
QTreeWidgetItem* paletteFolder;
QTreeWidgetItem* spritesFolder;

AssetBrowser::AssetBrowser(QWidget* parent) : QTreeWidget{parent}
{
    // Config
    {
        this->setContextMenuPolicy(Qt::CustomContextMenu);
        // this->setHeaderHidden(true);
        this->setHeaderLabel("Asset Browser");
    }

    // Create trees
    {
        paletteFolder = CreateFolder("Palettes");
        scriptsFolder = CreateFolder("Scripts");
        CreateFolder("SoundFX");
        spritesFolder = CreateFolder("Sprites");
    }

    // Populate trees
    {
        auto palettePath = SDK::GetProjectDataPath() + "/Palettes";
        PopulateTree(paletteFolder, palettePath, "*.pal", QIcon(":/icons/sfc.svg"));

        auto scriptsPath = SDK::GetProjectDataPath() + "/Scripts";
        PopulateTree(scriptsFolder, scriptsPath, "*.lua", QIcon(":/icons/lua.svg"));

        auto spritesPath = SDK::GetProjectDataPath() + "/Sprites";
        PopulateTree(spritesFolder, spritesPath, "*.png", qApp->style()->standardIcon(QStyle::SP_TrashIcon));
    }

    connect(this, &QTreeWidget::itemDoubleClicked, this, [this](QTreeWidgetItem* item, int column)
    {
        if (!item || item->childCount() > 0)
        {
            // It's a folder (has children), ignore
            return;
        }

        // Reconstruct full path
        QString fullPath;
        QTreeWidgetItem* current = item;
        QStringList pathParts;

        while (current && current->parent())
        {
            pathParts.prepend(current->text(0));
            current = current->parent();
        }

        fullPath = item->data(0, Qt::UserRole).toString();

        qDebug() << "Double-clicked file:" << fullPath;

        if (item->childCount() == 0 && item->text(0).endsWith(".lua", Qt::CaseInsensitive))
        {
            GetMainWindow()->OpenCodeEditor(fullPath);
        }
    });
}

QTreeWidgetItem *AssetBrowser::CreateFolder(const QString &name)
{
    auto folder = new QTreeWidgetItem(this);
    folder->setText(0, name);
    folder->setSizeHint(0, QSize(100, 20));
    folder->setIcon(0, qApp->style()->standardIcon(QStyle::SP_DirIcon));

    return folder;
}

void AssetBrowser::PopulateTree(QTreeWidgetItem* parentItem, const QString& folderPath, const char* extension, const QIcon& icon)
{
    QDir dir(folderPath);
    if (!dir.exists()) return;

    QDirIterator it(folderPath, QStringList() << extension, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext())
    {
        QString filePath = it.next();
        QFileInfo fileInfo(filePath);

        QString relativePath = dir.relativeFilePath(fileInfo.path());
        QTreeWidgetItem* currentParent = parentItem;

        if (relativePath != ".")
        {
            QStringList pathParts = relativePath.split('/', Qt::SkipEmptyParts);
            for (const QString& part : pathParts)
            {
                bool found = false;
                for (int i = 0; i < currentParent->childCount(); ++i)
                {
                    if (currentParent->child(i)->text(0) == part)
                    {
                        currentParent = currentParent->child(i);
                        found = true;
                        break;
                    }
                }
                if (!found)
                {
                    QTreeWidgetItem* folderItem = new QTreeWidgetItem(currentParent);
                    folderItem->setText(0, part);
                    folderItem->setIcon(0, qApp->style()->standardIcon(QStyle::SP_DirIcon));
                    folderItem->setSizeHint(0, QSize(100, 20));
                    currentParent = folderItem;
                }
            }
        }

        QTreeWidgetItem* fileItem = new QTreeWidgetItem(currentParent);
        fileItem->setText(0, fileInfo.fileName());
        fileItem->setIcon(0, icon);
        fileItem->setSizeHint(0, QSize(100, 20));
        fileItem->setData(0, Qt::UserRole, filePath);
    }
}
