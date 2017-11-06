#include "refreshthread.h"

refreshThread::refreshThread(QString path,QTreeWidget* treeWidget,QIcon dirhomeIcon,QIcon dirIcon,QIcon fileIcon)
{
    this->path=path;
    this->treeWidget=treeWidget;
    this->dirhomeIcon=dirhomeIcon;
    this->dirIcon=dirIcon;
    this->fileIcon=fileIcon;
}

void refreshThread::run()
{
    dir.setPath(path);
    if (dir.exists())
    {
        treeWidget->clear();
        treeWidget->setColumnCount(1); //设置列数
        treeWidget->setHeaderLabel("Bind Directory: "+path); //设置头的标题
        QTreeWidgetItem *root = new QTreeWidgetItem(treeWidget,QStringList(QString("root")));
        root->setIcon(0,dirhomeIcon);
        root->setToolTip(0,path);
        AddtoTree(root,dir);
        root->setExpanded(true);
    }
    else
    {
        treeWidget->clear();
        treeWidget->setColumnCount(1); //设置列数
        treeWidget->setHeaderLabel("Bind Directory: No"); //设置头的标题
        QTreeWidgetItem *root = new QTreeWidgetItem(treeWidget,QStringList(QString("root")));
        root->setIcon(0,dirhomeIcon);
        root->setToolTip(0,"root");
    }
}

void refreshThread::AddtoTree(QTreeWidgetItem* root,QDir dir)
{
    if (!dir.exists())
        return ;
    dir.setFilter(QDir::Dirs|QDir::Files);
    dir.setSorting(QDir::DirsFirst);
    QFileInfoList list = dir.entryInfoList();
    int i = 0;
    bool bIsDir;
    do{
        QFileInfo fileInfo = list.at(i);
        if(fileInfo.fileName()=="."||fileInfo.fileName()=="..")
        {
            i++;
            continue;
        }
        bIsDir = fileInfo.isDir();
        if (bIsDir)
        {
            QTreeWidgetItem *newitem = new QTreeWidgetItem(root,QStringList(fileInfo.fileName()));
            newitem->setIcon(0,dirIcon);
            newitem->setToolTip(0,fileInfo.filePath());
            root->addChild(newitem);
            AddtoTree(newitem,QDir(fileInfo.filePath()));
        }
        else
        {
            QTreeWidgetItem *newitem = new QTreeWidgetItem(root,QStringList(fileInfo.fileName()));
            newitem->setIcon(0,fileIcon);
            newitem->setToolTip(0,fileInfo.filePath());
            root->addChild(newitem);
        }
        i++;
    }
    while(i<list.size());

}
