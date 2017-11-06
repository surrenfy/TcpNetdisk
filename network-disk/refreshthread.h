#ifndef REFRESHTHREAD_H
#define REFRESHTHREAD_H

#include <QThread>
#include <QTreeWidget>
#include <QDir>
#include <QFile>

class refreshThread:public QThread
{
    Q_OBJECT
public:
    refreshThread(QString,QTreeWidget*,QIcon,QIcon,QIcon);
    void run();
private:
    QString path;
    QTreeWidget* treeWidget;
    QDir dir;
    QIcon dirhomeIcon;
    QIcon dirIcon;
    QIcon fileIcon;
    void AddtoTree(QTreeWidgetItem*,QDir);
};

#endif // REFRESHTHREAD_H
