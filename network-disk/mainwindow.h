#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>
#include <QTextStream>
#include <cstring>
#include <QResizeEvent>
#include <QFileDialog>
#include <QTreeWidget>
#include <QDesktopServices>
#include <QIcon>
#include <QMenu>
#include <QFont>
#include <QByteArray>
#include <QCryptographicHash>
#include <QTcpSocket>
#include <QDateTime>
#include <QTimer>
#include <QColorDialog>
#include "logindialog.h"
#include "refreshthread.h"
#include "transportdialog.h"
#include <cstring>


#define MAX_FILE_SIZE 1024*1024*1024
#define BLOCK_SIZE    1024*64
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    friend class refreshThread;

private slots:
    void login();
    void relogin();
    void rebind();
    void unbind();
    void refresh();
    void opendir();
    void logout();
    void setcolor();
    void on_treeWidget_itemSelectionChanged();
    void on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void setRefreshActionEnable();
    void on_connectserver();
    void on_login();
    void on_register();
    void on_recvfile(QFile*,QByteArray md5,int);
    void serverconn();
    void serverdisconn();
    void update();
    void setrgb();
private:
    Ui::MainWindow *ui;
    bool logined;
    bool saveuser;
    bool serverconnecting;
    QColor color;
    QColorDialog* colordialog;
    QTimer *timer;
	QDateTime stocupdatetime;
    QIcon icon;
    QString path;
    QString username;
    QString password;
    QString serverip;
	QString maincfgdir;
	QString mainlogdir;
    QString usercfgdir;
    QString userlogdir;
    QStringList tempfileList;
    unsigned short port;
    QDir dir;
	QDir maindir;
    QFile* maincfg;
    QFile* usercfg;
    QFile* mainlog;
    QFile* userlog;
    QAction* OpendirAction;
    QAction* RefreshAction;
    QAction* LoginAction;
    QAction* LogoutAction;
    QAction* SetcolorAction;
    QAction* RebindAction;
    QAction* ConnectAction;
    QAction* UnbindActoin;
    QAction* TransportAction;
    QMenu*   LoginMenu;
    QMenu*   SettingsMenu;
    QFont*   font;
    bool TryAutoLogin(QString);
    bool LoginInit(QFile&);
    void AddtoTree(QTreeWidgetItem *,QDir);
    int  QByteArraytoInt(QByteArray);
    QByteArray InttoQByteArray(int);
    LoginDialog* logindialog;
    TransportDialog* transportdialog;
    refreshThread* RefreshThread;
    QTcpSocket*  tcpsocket;
    void readusercfg();
    //-10=消息未发送 -11=消息发送了没回复 -12=消息接收了但未通过md5校验 10=服务器未定义的回应
    int tcp_login(QByteArray&,QByteArray&); //0=服务端请求重新发送 -2=用户名未注册 -1=密码错误 1=正常登录
    int tcp_register(QByteArray&,QByteArray&); //0=服务端请求重新发送 -1=用户名已存在 1=注册成功并正常登录
    int tcp_logout(QByteArray&); //0=服务端请求重新发送 1=正常登出 2=服务端请求重新登录
    int tcp_initialstoc_head(); //-1=服务端拒绝请求 0=服务端请求重新发送 1=服务端同意请求 2=服务端请求重新登录
    int tcp_initialstoc_dir(int num); //0=服务端请求重新发送 1=正常操作（目录存在就略过，否则创建） 2=服务端请求重新登录 -1=服务端表示没有这个编号的目录可以发
    int tcp_initialstoc_file(int num); //0=服务端请求重新发送 1=文件名相同，内容相同 2=服务端请求重新登录 -1=文件名相同，内容不同，已自动将客户端原来文件改名并接收新文件 -2=文件名不同，内容不同，已自动接收文件
    int tcp_recv_file(QByteArray md5,int startfrom,int &len,QByteArray&content); //0=服务端请求重新发送 1=正常接收
    int tcp_initialctos_head();// -1=服务端拒绝请求 0=服务端请求重新发送 1=服务端同意请求 2=服务端请求重新登录
	int tcp_initialctos_dir(QFileInfoList&dirlist,int &num);
	int tcp_initialctos_file(QFileInfoList&filelist,int &num);
    int tcp_initialstoc();
    int tcp_initialctos();
	int tcp_initialctos_all(QFileInfoList&,QFileInfoList&);
    int tcp_updatestoc();
    int tcp_updatestoc_head();
    int tcp_updatestoc_all(QDir);
	int tcp_updatestoc_dir(QString);
	int tcp_updatestoc_file(QFileInfo);
	int tcp_updatestoc_newdir(QFileInfo,int);
	int tcp_updatestoc_newfile(QFileInfo,int);
	int tcp_updatectos();
	int tcp_updatectos_head();
    int tcp_updatectos_all(QFileInfoList&,QFileInfoList&);
	int tcp_rebind();
    void addlist(QDir dir,QFileInfoList&dirlist,QFileInfoList&filelist);
    void checkTempfileList();
protected:
    void closeEvent(QCloseEvent *event);
    void resizeEvent(QResizeEvent *event);
};

#endif // MAINWINDOW_H
