#ifndef ASSETBROWSER_H
#define ASSETBROWSER_H

#include <QTreeWidget>

class AssetBrowser : public QTreeWidget
{
    Q_OBJECT
public:
    AssetBrowser(QWidget* parent = nullptr);

private:
    QTreeWidgetItem* CreateFolder(const QString& name);
};

#endif // ASSETBROWSER_H
