#ifndef ASSETBROWSER_H
#define ASSETBROWSER_H

#include <QTreeWidget>

class AssetBrowser : public QTreeWidget
{
    Q_OBJECT
public:
    AssetBrowser(QWidget* parent = nullptr);

private:
    auto CreateFolder(const QString& name) -> QTreeWidgetItem*;
    auto PopulateTree(QTreeWidgetItem* parentItem, const QString& folderPath, const char* extension, const QIcon& icon) -> void;
};

#endif // ASSETBROWSER_H
