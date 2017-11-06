#include "mainwindow.h"
#include "ui_mainwindow.h"

int toint(QByteArray a)
{
    QString s=a.mid(0,10);
    return atoi(s.toLatin1().data());
}

int toint(QString a)
{
    QString s=a.mid(0,10);
    return atoi(s.toLatin1().data());
}

void sleep(int msec)
{
    QTime dieTime=QTime::currentTime().addMSecs(msec);
    while(QTime::currentTime()<dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));

    ui->treeWidget->setFrameStyle(QFrame::NoFrame);//无边框
    ui->treeWidget->setAttribute(Qt::WA_TranslucentBackground);//背景透明
    setWindowIcon(style()->standardIcon(QStyle::SP_DriveHDIcon));//设置图标
    icon=style()->standardIcon(QStyle::SP_DriveHDIcon);
    mainlog=NULL;
    maincfg=NULL;
    userlog=NULL;
    usercfg=NULL;
    saveuser=false;
    font=new QFont;
    font->setFamily("Comic Sans MS");
    font->setPointSize(9);
    color.setRgb(240,240,240);
    maindir.setPath(dir.currentPath());
    maincfgdir=maindir.absoluteFilePath("main.cfg");
    mainlogdir=maindir.absoluteFilePath("main.log");
    stocupdatetime=QDateTime::fromString("2000-01-01 00:00:00","yyyy-MM-dd HH:mm:ss");
    qDebug()<<"initial time from server"<<stocupdatetime.toString("yyyy-MM-dd HH:mm:ss");
    transportdialog=new TransportDialog(icon);
    colordialog=new QColorDialog();
    connect(colordialog,SIGNAL(accepted()),this,SLOT(setrgb()));
    OpendirAction=new QAction(style()->standardIcon(QStyle::SP_DirIcon),tr("&Open..."),this);
    OpendirAction->setShortcut(QKeySequence::Open);
    OpendirAction->setStatusTip(tr("Open the selected directory"));
    ui->mainToolBar->addAction(OpendirAction);
    OpendirAction->setEnabled(false);

    RefreshAction=new QAction(style()->standardIcon(QStyle::SP_BrowserReload),tr("&Refresh"),this);
    RefreshAction->setShortcut(QKeySequence::Refresh);
    RefreshAction->setStatusTip(tr("Refresh the bind directory"));
    ui->mainToolBar->addAction(RefreshAction);

    ConnectAction=new QAction(style()->standardIcon(QStyle::SP_DriveNetIcon),tr("&Connect"),this);
    ConnectAction->setStatusTip(tr("Connect to the server"));
    ui->mainToolBar->addAction(ConnectAction);

    TransportAction=new QAction(style()->standardIcon(QStyle::SP_CommandLink),tr("&Transporting files"),this);
    TransportAction->setStatusTip(tr("Show the transporting files table"));
    ui->mainToolBar->addAction(TransportAction);

    LoginAction=new QAction(tr("Login"),this);
    LoginAction->setStatusTip(tr("Login or Register"));

    LogoutAction=new QAction(tr("Logout"),this);
    LogoutAction->setStatusTip(tr("Logout"));

    SetcolorAction=new QAction(tr("Setcolor"),this);
    SetcolorAction->setStatusTip(tr("Set the client color"));

    RebindAction=new QAction(tr("Rebind directory.."),this);
    RebindAction->setStatusTip(tr("Rebind your main directory"));

    UnbindActoin=new QAction(tr("Unbind directory"),this);
    UnbindActoin->setStatusTip(tr("Unbind your main directory"));

    LoginMenu=new QMenu(tr("Login"));
    LoginMenu->addAction(LoginAction);
    LoginMenu->addSeparator();
    LoginMenu->addAction(LogoutAction);
    ui->menuBar->addMenu(LoginMenu);

    SettingsMenu=new QMenu(tr("Settings"));
    SettingsMenu->addAction(UnbindActoin);
    SettingsMenu->addAction(RebindAction);
    SettingsMenu->addAction(RefreshAction);
    SettingsMenu->addAction(SetcolorAction);
    ui->menuBar->addMenu(SettingsMenu);
    LoginMenu->setFont(*font);
    SettingsMenu->setFont(*font);
    LogoutAction->setEnabled(false);
    RebindAction->setEnabled(false);
    SetcolorAction->setEnabled(true);
    ConnectAction->setEnabled(false);
    UnbindActoin->setEnabled(false);
    RefreshAction->setEnabled(true);
    LoginAction->setEnabled(true);
    connect(RefreshAction, SIGNAL(triggered()), this, SLOT(refresh()));
    connect(OpendirAction, SIGNAL(triggered()), this, SLOT(opendir()));
    connect(LoginAction, SIGNAL(triggered()), this, SLOT(login()));
    connect(LogoutAction, SIGNAL(triggered()), this, SLOT(logout()));
    connect(RebindAction, SIGNAL(triggered()), this, SLOT(rebind()));
    connect(SetcolorAction, SIGNAL(triggered()), this, SLOT(setcolor()));
    connect(ConnectAction, SIGNAL(triggered()),this,SLOT(on_connectserver()));
    connect(UnbindActoin, SIGNAL(triggered()), this, SLOT(unbind()));
    connect(TransportAction,SIGNAL(triggered()),transportdialog,SLOT(show()));

    logined=false;
    username="";
    password="";
    path="";


    mainlog=new QFile(mainlogdir);
    maincfg=new QFile(maincfgdir);
    if(!mainlog->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) {
        QMessageBox::warning(this,"warning"," Can't open \"main.log\"    ");
    }

    mainlog->close();
    if(!maincfg->open(QIODevice::ReadWrite | QIODevice::Text)) {
        QMessageBox::warning(this,"warning"," Can't open \"main.cfg\"    ");
        exit(0);
    }
    else{
        int px,py,h,w,temp;
        QString line;
        QTextStream in(maincfg);
        px=x();
        py=y();
        h=height();
        w=width();
        //bool ok;
        while(!in.atEnd())
        {
            line=in.readLine();
            temp=-1;
            if (line.indexOf("PreferPosX:",Qt::CaseInsensitive)>=0){
                temp=toint(line.mid(line.indexOf("PreferPosX:",Qt::CaseInsensitive)+int(strlen("PreferPosX:"))));
                if (temp>=0)
                    px=temp;
            }
            if (line.indexOf("PreferPosY:",Qt::CaseInsensitive)>=0){
                temp=toint(line.mid(line.indexOf("PreferPosY:",Qt::CaseInsensitive)+int(strlen("PreferPosY:"))));
                if (temp>=0)
                    py=temp;
            }
            if (line.indexOf("PreferHeight:",Qt::CaseInsensitive)>=0){
                temp=toint(line.mid(line.indexOf("PreferHeight:",Qt::CaseInsensitive)+int(strlen("PreferHeight:"))));
                if (temp>=0)
                    h=temp;
            }
            if (line.indexOf("PreferWidth:",Qt::CaseInsensitive)>=0){
                temp=toint(line.mid(line.indexOf("PreferWidth:",Qt::CaseInsensitive)+int(strlen("PreferWidth:"))));
                if (temp>=0)
                    w=temp;
            }
            if (line.indexOf("LastLogin:",Qt::CaseInsensitive)>=0){
                username=line.mid(line.indexOf("LastLogin:",Qt::CaseInsensitive)+int(strlen("LastLogin:")));
            }
            if (line.indexOf("Password:",Qt::CaseInsensitive)>=0){
                password=line.mid(line.indexOf("Password:",Qt::CaseInsensitive)+int(strlen("Password:")));
            }
            if (line.indexOf("ServerIP:",Qt::CaseInsensitive)>=0){
                serverip=line.mid(line.indexOf("ServerIP:",Qt::CaseInsensitive)+int(strlen("ServerIP:")));
            }
            if (line.indexOf("ServerPort:",Qt::CaseInsensitive)>=0){
                port=line.mid(line.indexOf("ServerPort:",Qt::CaseInsensitive)+int(strlen("ServerPort:"))).toUShort();
            }
        }
        setGeometry(px,py,w,h);
    }
    maincfg->close();
    qRegisterMetaType<QVector<int>>("QVector<int>");
    qRegisterMetaType<Qt::Orientation>("Qt::Orientation");
    qRegisterMetaType<QItemSelection>("QItemSelection");
    refresh();
    serverconnecting=false;
    tcpsocket=new QTcpSocket();
    connect(tcpsocket,SIGNAL(connected()),this,SLOT(serverconn()));
    connect(tcpsocket,SIGNAL(disconnected()),this,SLOT(serverdisconn()));
    on_connectserver();
}

void MainWindow::readusercfg()
{
    if(usercfg&&usercfg->open(QIODevice::ReadWrite | QIODevice::Text))
    {
        int r=-1,g=-1,b=-1;
        QString line;
        QString userpath;
        QTextStream in(usercfg);
        tempfileList.clear();
        while(!in.atEnd())
        {
            line=in.readLine();
            if (line.indexOf("Path:",Qt::CaseInsensitive)>=0){
                userpath=line.mid(line.indexOf("Path:",Qt::CaseInsensitive)+int(strlen("Path:")));
                QDir userdir(userpath);
                if (userdir.exists()&&userdir.isAbsolute())
                {
                    path=userpath;
                    dir.setCurrent(path);
                    dir.cd(".");
                    UnbindActoin->setEnabled(true);
                    refresh();
                }
            }
            if (line.indexOf("R:",Qt::CaseInsensitive)>=0){
                r=toint(line.mid(line.indexOf("R:",Qt::CaseInsensitive)+int(strlen("R:"))));
            }
            if (line.indexOf("G:",Qt::CaseInsensitive)>=0){
                g=toint(line.mid(line.indexOf("G:",Qt::CaseInsensitive)+int(strlen("G:"))));
            }
            if (line.indexOf("B:",Qt::CaseInsensitive)>=0){
                b=toint(line.mid(line.indexOf("B:",Qt::CaseInsensitive)+int(strlen("B:"))));
            }
            if (line.indexOf("Tempfile:",Qt::CaseInsensitive)>=0){
                tempfileList.append(line.mid(line.indexOf("Tempfile:",Qt::CaseInsensitive)+int(strlen("Tempfile:"))));
            }
            if (r>=0 && g>=0 && b>=0 && r<256 && g<256 && b<256)
            {
                color.setRgb(r,g,b);
                QString style=QString("background-color: rgb(%1,%2,%3)").arg(r).arg(g).arg(b);
                this->setStyleSheet(style);
                this->ui->treeWidget->setStyleSheet(style);
            }
            checkTempfileList();
        }
        usercfg->close();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton button;
    button = QMessageBox::question(this, tr("exit"),
                                   QString(tr("Are you sure to exit?    ")),
                                   QMessageBox::Yes | QMessageBox::No);
    if (button == QMessageBox::No) {
        event->ignore();
    }
    else if (button == QMessageBox::Yes) {
        event->accept();
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    ui->treeWidget->setGeometry(15,10,width()-30,height()-20);
}

void MainWindow::setcolor()
{
    colordialog->show();
}

void MainWindow::setrgb()
{
    color=colordialog->currentColor();
    int r,g,b;
    color.getRgb(&r,&g,&b);
    QString style=QString("background-color: rgb(%1,%2,%3)").arg(r).arg(g).arg(b);
    this->setStyleSheet(style);
    this->ui->treeWidget->setStyleSheet(style);
}

void MainWindow::checkTempfileList()
{
    int i;
    for (i=0;i<tempfileList.size();i++)
    {
        QFile file(tempfileList.at(i));
        if (!file.exists()){
            tempfileList.removeAt(i--);
            continue;
        }
        if (!file.open(QIODevice::ReadOnly ))
            continue;
        QString md5=QCryptographicHash::hash(file.readAll(),QCryptographicHash::Md5).toHex();
        file.close();
        if (file.fileName().mid(0,32)!=md5){
            if(file.fileName().lastIndexOf(".sjtempfile")+int(strlen(".sjtempfile")==file.fileName().length()))//失效的临时文件
                QFile::remove(QFileInfo(file).absoluteFilePath());
            tempfileList.removeAt(i--);
        }
    }
}

void MainWindow::on_treeWidget_itemSelectionChanged()
{
    if(ui->treeWidget->selectedItems().empty()||!QDir(ui->treeWidget->selectedItems().first()->toolTip(0)).exists())//未选中或者目录不存在
        OpendirAction->setDisabled(true);
    else OpendirAction->setDisabled(false);
}

void MainWindow::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    QDesktopServices::openUrl(QUrl("file:///"+item->toolTip(column), QUrl::TolerantMode));
    /*
    QFile theFile(item->toolTip(column));
    if (theFile.open(QIODevice::ReadOnly))
    {
        theFile->open(QIODevice::WriteOnly);
        QByteArray content=theFile.readAll();
        theFile->close();
        QByteArray md5 = QCryptographicHash::hash(content,QCryptographicHash::Md5);
        qDebug() << md5.toHex().constData();
    }
    theFile.close();
*/

}

void MainWindow::opendir()
{
    if(ui->treeWidget->selectedItems().empty())
        return;
    on_treeWidget_itemDoubleClicked(ui->treeWidget->selectedItems().first(), 0);
}

MainWindow::~MainWindow()
{
    if(maincfg->open(QIODevice::ReadWrite | QIODevice::Text |QIODevice::Truncate))
    {
        QTextStream out(maincfg);
        if (saveuser&&logined)
        {
            out<<"LastLogin:"   <<username                  <<endl;
            out<<"Password:"    <<password                  <<endl;
        }
        else
        {
            out<<"LastLogin:"<<endl;
            out<<"Password:"<<endl;
        }
        out<<"PreferPosX:"  <<QString::number(x())      <<endl;
        out<<"PreferPosY:"  <<QString::number(y())      <<endl;
        out<<"PreferWidth:" <<QString::number(width())  <<endl;
        out<<"PreferHeight:"<<QString::number(height()) <<endl;
        out<<"ServerIP:"    <<serverip <<endl;
        out<<"ServerPort:"  <<port <<endl;
        maincfg->close();
    }
    if (logined&&usercfg->open(QIODevice::ReadWrite | QIODevice::Text |QIODevice::Truncate))
    {
        QTextStream out(usercfg);
        int r,g,b;
        color.getRgb(&r,&g,&b);
        out<<"Path:"<<path<<endl;
        out<<"R:"<<r<<endl;
        out<<"G:"<<g<<endl;
        out<<"B:"<<b<<endl;
        while(!tempfileList.empty())
        {
            out<<"Tempfile:"<<tempfileList.first();
            tempfileList.removeFirst();
        }
        maincfg->close();
    }
    delete ui;
    if (mainlog){
        mainlog->deleteLater();
    }
    if (maincfg){
        maincfg->deleteLater();
    }
    if (userlog){
        userlog->deleteLater();
    }
    if (usercfg){
        usercfg->deleteLater();
    }
    if (logindialog)
    {
        logindialog->deleteLater();
    }
    if (tcpsocket)
    {
        tcpsocket->disconnectFromHost();
        tcpsocket->deleteLater();
    }
}

void MainWindow::login()
{
    if (!serverconnecting)
        on_connectserver();
    if (!serverconnecting)
        return;
    if (username!="")
    {
        saveuser=true;
        relogin();
    }
    else
    {
        logindialog=new LoginDialog(icon);
        logindialog->move(x()+120,y()+120);
        logindialog->show();
        connect(logindialog->login_okButton,SIGNAL(clicked()),this,SLOT(on_login()));
        connect(logindialog->register_okButton,SIGNAL(clicked()),this,SLOT(on_register()));
    }
}

void MainWindow::relogin()
{
    logindialog=new LoginDialog(icon);
    //禁用其他功能 只能通过再次登陆解禁
    LogoutAction->setEnabled(false);
    RebindAction->setEnabled(false);
    ConnectAction->setEnabled(false);
    UnbindActoin->setEnabled(false);
    LoginAction->setEnabled(true);
    logined=false;
    LoginAction->setStatusTip(tr("Login or Register"));
    LoginAction->setText("Login");

    if (usercfg&&usercfg->open(QIODevice::ReadWrite | QIODevice::Text |QIODevice::Truncate))
    {
        QTextStream out(usercfg);
        int r,g,b;
        color.getRgb(&r,&g,&b);
        out<<"Path:"<<path<<endl;
        out<<"R:"<<r<<endl;
        out<<"G:"<<g<<endl;
        out<<"B:"<<b<<endl;
        while(!tempfileList.empty())
        {
            out<<"Tempfile:"<<tempfileList.first();
            tempfileList.pop_front();
        }
        usercfg->close();
    }
    path="";
    dir.setCurrent(maindir.path());
    if (userlog&&userlog->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
        QString timestr = time.toString("yyyy-MM-dd HH:mm:ss: "); //设置显示格式
        timestr+="user logout\n";
        userlog->write(timestr.toLatin1());
        userlog->close();
    }
    if (usercfg)
        usercfg->deleteLater();
    if (userlog)
        userlog->deleteLater();
    if (!saveuser)
    {
        username="";
        password="";
    }
    else{
        logindialog->login_remember->setChecked(true);
    }
    if (!serverconnecting)
        on_connectserver();
    if (!serverconnecting)
        return;

    logindialog->move(x()+120,y()+120);
    logindialog->show();
    qDebug()<<"username:"<<username;
    logindialog->login_nameEdit->setText(username);
    logindialog->login_passwordEdit->setText(password);
    logindialog->login_suggest->setText("Please relogin");
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::darkRed);
    logindialog->login_suggest->setPalette(pa);
    connect(logindialog->login_okButton,SIGNAL(clicked()),this,SLOT(on_login()));
    connect(logindialog->register_okButton,SIGNAL(clicked()),this,SLOT(on_register()));

}

void MainWindow::rebind()
{
    timer->stop();
    QString temp_path = QFileDialog::getExistingDirectory(this,"select a new bind directory","./");

    if (temp_path!=""&&temp_path!=path){
        path=temp_path;
        dir.setCurrent(path);
        qDebug()<<"bind dir"<<path;
        if (userlog&&userlog->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        {
            QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
            QString timestr = time.toString("yyyy-MM-dd HH:mm:ss: "); //设置显示格式
            timestr+="user rebind dir:";
            timestr+=path;
            timestr+="\n";
            userlog->write(timestr.toLatin1());
            userlog->close();
        }
        tcp_rebind();
        UnbindActoin->setEnabled(true);
        RefreshAction->setEnabled(false);
        tcp_initialstoc();
        tcp_initialctos();
        refresh();
    }
}

void MainWindow::unbind()
{
    timer->stop();
    path="";
    dir.setCurrent(maindir.path());
    refresh();
    tcp_rebind();
    qDebug()<<"unbind dir";
    if (userlog&&userlog->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
        QString timestr = time.toString("yyyy-MM-dd HH:mm:ss: "); //设置显示格式
        timestr+="user unbind dir\n";
        userlog->write(timestr.toLatin1());
        userlog->close();
    }
    UnbindActoin->setEnabled(false);
}

void MainWindow::logout()
{
    timer->stop();
    QByteArray username=this->username.toLatin1();
    int res=tcp_logout(username);
    if (res==1)
    {
        if (usercfg&&usercfg->open(QIODevice::ReadWrite | QIODevice::Text |QIODevice::Truncate))
        {
            QTextStream out(usercfg);
            int r,g,b;
            color.getRgb(&r,&g,&b);
            out<<"Path:"<<path<<endl;
            out<<"R:"<<r<<endl;
            out<<"G:"<<g<<endl;
            out<<"B:"<<b<<endl;
            while(!tempfileList.empty())
            {
                out<<"Tempfile:"<<tempfileList.first();
                tempfileList.pop_front();
            }
            usercfg->close();
        }
        if (mainlog->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        {
            QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
            QString timestr = time.toString("yyyy-MM-dd HH:mm:ss: "); //设置显示格式
            timestr+="user:";
            timestr+=username;
            timestr+=" logout\n";
            mainlog->write(timestr.toLatin1());
            mainlog->close();
        }
        LogoutAction->setEnabled(false);
        RebindAction->setEnabled(false);
        ConnectAction->setEnabled(false);
        UnbindActoin->setEnabled(false);
        LoginAction->setEnabled(true);
        logined=false;
        path="";
        dir.setCurrent(maindir.path());
        LoginAction->setStatusTip(tr("Login or Register"));
        LoginAction->setText("Login");
        if (userlog&&userlog->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        {
            QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
            QString timestr = time.toString("yyyy-MM-dd HH:mm:ss: "); //设置显示格式
            timestr+="user logout\n";
            userlog->write(timestr.toLatin1());
            userlog->close();
        }

        if (!saveuser)
        {
            username="";
            password="";
        }
        refresh();
    }

}

void MainWindow::refresh()
{

    RefreshAction->setEnabled(false);
    RefreshThread=new refreshThread(path,ui->treeWidget,style()->standardIcon(QStyle::SP_DirHomeIcon),style()->standardIcon(QStyle::SP_DirIcon),style()->standardIcon(QStyle::SP_FileIcon));
    RefreshThread->start();

    connect(RefreshThread,SIGNAL(finished()),this,SLOT(setRefreshActionEnable()));
    connect(RefreshThread,SIGNAL(finished()),RefreshThread,SLOT(deleteLater()));
}

void MainWindow::setRefreshActionEnable()
{
    RefreshAction->setEnabled(true);
    if (RefreshThread)
        RefreshThread->deleteLater();
    if (path!=""&&logined)

        timer->start(10000);
}

void MainWindow::on_connectserver()
{
    tcpsocket->connectToHost(serverip, port);
    ConnectAction->setEnabled(false);
    if (!tcpsocket->waitForConnected(5000))
    {
        QMessageBox::critical(this,"error"," Can't connect to server!");
        if (mainlog->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
            QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
            QString timestr = time.toString("yyyy-MM-dd HH:mm:ss: "); //设置显示格式
            timestr+="Connect to server failed\n";
            mainlog->write(timestr.toLatin1());

        }
        mainlog->close();
        ConnectAction->setEnabled(true);
    }
}

void MainWindow::on_login()
{
    QByteArray arg1 = logindialog->login_nameEdit->text().toLatin1();
    QByteArray arg2 = QCryptographicHash::hash(logindialog->login_passwordEdit->text().toLatin1(),QCryptographicHash::Md5).toHex();
    QByteArray tp=logindialog->login_passwordEdit->text().toLatin1();
    int res;
    res=tcp_login(arg1,arg2);
    if(res==-10)
    {
        logindialog->login_suggest->setText("Can't send message to server");
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::darkRed);
        logindialog->login_suggest->setPalette(pa);
        qDebug()<<"Can't send message to server";
    }
    else if(res==-11)
    {
        logindialog->login_suggest->setText("Can't recv message from server");
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::darkRed);
        logindialog->login_suggest->setPalette(pa);
        qDebug()<<"Can't recv message from server";
    }
    else if (res==-12||res==0)
    {
        logindialog->login_suggest->setText("Netwrok failure,please resend later");
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::darkRed);
        logindialog->login_suggest->setPalette(pa);
        qDebug()<<"Netwrok failure";
    }
    else if (res==1)
    {
        logined=true;
        logindialog->close();
        username=arg1;
        password=tp;
        saveuser=logindialog->login_remember->isChecked();
        LoginAction->setText(username);//将loginaction改名
        LoginAction->setStatusTip("the user has logined");//更改说明
        LoginAction->setDisabled(true);
        LogoutAction->setEnabled(true);
        RebindAction->setEnabled(true);
        qDebug()<<"user "<<username<<"logined";
        if (mainlog->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        {
            QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
            QString timestr = time.toString("yyyy-MM-dd HH:mm:ss: "); //设置显示格式
            timestr+="user:";
            timestr+=username;
            timestr+=" logined\n";
            mainlog->write(timestr.toLatin1());
            mainlog->close();
        }
        userlogdir=maindir.absoluteFilePath(username+".userlog");
        usercfgdir=maindir.absoluteFilePath(username+".usercfg");
        userlog=new QFile(userlogdir);
        usercfg=new QFile(usercfgdir);
        if (userlog&&userlog->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        {
            QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
            QString timestr = time.toString("yyyy-MM-dd HH:mm:ss: "); //设置显示格式
            timestr+="user logined\n";
            userlog->write(timestr.toLatin1());
            userlog->close();
        }
        readusercfg();
    }
    else if (res==-1)
    {
        logindialog->login_suggest->setText("Wrong password");
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::darkRed);
        logindialog->login_suggest->setPalette(pa);
        qDebug()<<"wrong password";
        if (mainlog->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        {
            QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
            QString timestr = time.toString("yyyy-MM-dd HH:mm:ss: "); //设置显示格式
            timestr+="user:";
            timestr+=username;
            timestr+=" send a wrong password\n";
            mainlog->write(timestr.toLatin1());
            mainlog->close();
        }
    }
    else if (res==-2)
    {
        logindialog->login_suggest->setText("Unregistered username");
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::darkRed);
        logindialog->login_suggest->setPalette(pa);
        qDebug()<<"unregistered username";
    }
    else
    {
        qDebug()<<"server reply undefined";
    }
}

void MainWindow::on_register()
{
    QByteArray arg1 = logindialog->register_nameEdit->text().toLatin1();
    QByteArray arg2 = QCryptographicHash::hash(logindialog->register_passwordEdit->text().toLatin1(),QCryptographicHash::Md5).toHex();
    QByteArray tp=logindialog->register_passwordEdit->text().toLatin1();
    int res = 0;
    res=tcp_register(arg1,arg2);
    if(res==-10)
    {
        logindialog->register_suggest->setText("Can't send message to server");
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::darkRed);
        logindialog->register_suggest->setPalette(pa);
        qDebug()<<"Can't send message to server";
    }
    else if(res==-11)
    {
        logindialog->register_suggest->setText("Can't recv message from server");
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::darkRed);
        logindialog->register_suggest->setPalette(pa);
        qDebug()<<"Can't recv message from server";
    }
    else if (res==-12||res==0)
    {
        logindialog->register_suggest->setText("Netwrok failure,please resend later");
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::darkRed);
        logindialog->register_suggest->setPalette(pa);
        qDebug()<<"Netwrok failure";
    }
    else if (res==1)
    {
        logined=true;
        logindialog->close();
        username=arg1;
        password=tp;
        saveuser=logindialog->login_remember->isChecked();
        LoginAction->setText(username);//将loginaction改名
        LoginAction->setStatusTip("the user has logined");//更改说明
        LoginAction->setDisabled(true);
        LogoutAction->setEnabled(true);
        RebindAction->setEnabled(true);
        qDebug()<<"user "<<username<<"logined";
        if (mainlog->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        {
            QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
            QString timestr = time.toString("yyyy-MM-dd HH:mm:ss: "); //设置显示格式
            timestr+="user:";
            timestr+=username;
            timestr+=" logined\n";
            mainlog->write(timestr.toLatin1());
            mainlog->close();
        }
        userlogdir=maindir.absoluteFilePath(username+".userlog");
        usercfgdir=maindir.absoluteFilePath(username+".usercfg");
        userlog=new QFile(userlogdir);
        usercfg=new QFile(usercfgdir);
        if (userlog&&userlog->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        {
            QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
            QString timestr = time.toString("yyyy-MM-dd HH:mm:ss: "); //设置显示格式
            timestr+="user logined\n";
            userlog->write(timestr.toLatin1());
            userlog->close();
        }
        readusercfg();
    }
    else if (res==-1)
    {
        logindialog->register_suggest->setText("The name is already exist");
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::darkRed);
        logindialog->register_suggest->setPalette(pa);
        qDebug()<<"The name is already exist";

    }
    else
    {
        qDebug()<<"server reply undefined";
    }
}

int  MainWindow::tcp_login(QByteArray &username,QByteArray &password)
{
    QByteArray sendmsg="";//发送的完整消息
    QByteArray recvmsg="";//收到的完整消息
    QByteArray sendcontent="";//发送的正文
    QByteArray recvcontent="";//收到的正文
    int sendlen;
    int recvlen;

    if (tcpsocket->isWritable())//如果允许发消息
    {
        sendcontent+="request:loginin,username:";
        sendcontent+=username;
        sendcontent+=",password:";
        sendcontent+=password;
        sendlen=sendcontent.length();

        sendmsg+=InttoQByteArray(sendlen);
        sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
        sendmsg+=sendcontent;
        tcpsocket->write(sendmsg);
        qDebug()<<"send: "<<sendcontent;
    }
    else
        return -10;
    if (tcpsocket->waitForReadyRead(5000))//如果5s内有消息收到
    {
        //验证消息的正确性
        recvmsg=tcpsocket->readAll();while (tcpsocket->waitForReadyRead(200)) recvmsg+=tcpsocket->readAll();qDebug()<<"recv bytes "<<recvmsg.length();
        if (recvmsg.length()<36)//收到消息不满36字节
            return -12;
        recvlen=QByteArraytoInt(recvmsg.mid(0,4));
        if (recvmsg.length()<36+recvlen)//收到消息长度不对
            return -12;
        recvcontent=recvmsg.mid(36,recvlen);
        if (QCryptographicHash::hash(recvcontent,QCryptographicHash::Md5).toHex()!=recvmsg.mid(4,32))//收到消息md5不对
            return -12;
        qDebug()<<"recv: "<<recvcontent;
        //确认消息含义
        if (recvcontent.mid(0,strlen("request:pardon"))=="request:pardon")//如果服务器要求重发
            return 0;
        if (recvcontent.mid(0,strlen("response:loginin"))=="response:loginin")//如果服务器正常回应
        {
            int code=toint(recvcontent.mid(int(strlen("response:loginin,code:")),10));
            return code;
        }
        else return 10;
    }
    else
        return -11;
}

int  MainWindow::tcp_register(QByteArray &username,QByteArray &password)
{
    QByteArray sendmsg="";//发送的完整消息
    QByteArray recvmsg="";//收到的完整消息
    QByteArray sendcontent="";//发送的正文
    QByteArray recvcontent="";//收到的正文
    int sendlen;
    int recvlen;

    if (tcpsocket->isWritable())//如果允许发消息
    {
        sendcontent+="request:loginup,username:";
        sendcontent+=username;
        sendcontent+=",password:";
        sendcontent+=password;
        sendlen=sendcontent.length();

        sendmsg+=InttoQByteArray(sendlen);
        sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
        sendmsg+=sendcontent;
        tcpsocket->write(sendmsg);
        qDebug()<<"send: "<<sendcontent;
    }
    else
        return -10;
    if (tcpsocket->waitForReadyRead(5000))//如果5s内有消息收到
    {
        //验证消息的正确性
        recvmsg=tcpsocket->readAll();while (tcpsocket->waitForReadyRead(200)) recvmsg+=tcpsocket->readAll();qDebug()<<"recv bytes "<<recvmsg.length();
        qDebug()<<"recv:"<<recvmsg.length()<<" bytes";
        if (recvmsg.length()<36)//收到消息不满36字节
            return -12;
        recvlen=QByteArraytoInt(recvmsg.mid(0,4));
        if (recvmsg.length()<36+recvlen)//收到消息长度不对
            return -12;
        recvcontent=recvmsg.mid(36,recvlen);
        if (QCryptographicHash::hash(recvcontent,QCryptographicHash::Md5).toHex()!=recvmsg.mid(4,32))//收到消息md5不对
            return -12;
        qDebug()<<"recv: "<<recvcontent;
        //确认消息含义
        if (recvcontent.mid(0,strlen("request:pardon"))=="request:pardon")//如果服务器要求重发
            return 0;
        if (recvcontent.mid(0,strlen("response:loginup"))=="response:loginup")//如果服务器正常回应
        {
            //bool ok;
            int code=toint(recvcontent.mid(int(strlen("response:loginup,code:")),10));
            //if (ok)
            return code;
            // else return 10;
        }
        else return 10;
    }
    else
        return -11;
}

int  MainWindow::tcp_logout(QByteArray&username)
{
    QByteArray sendmsg="";//发送的完整消息
    QByteArray recvmsg="";//收到的完整消息
    QByteArray sendcontent="";//发送的正文
    QByteArray recvcontent="";//收到的正文
    int sendlen;
    int recvlen;

    if (tcpsocket->isWritable())//如果允许发消息
    {
        sendcontent+="request:logout,username:";
        sendcontent+=username;
        sendlen=sendcontent.length();

        sendmsg+=InttoQByteArray(sendlen);
        sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
        sendmsg+=sendcontent;
        tcpsocket->write(sendmsg);
        qDebug()<<"send: "<<sendcontent;
    }
    else
        return -10;
    if (tcpsocket->waitForReadyRead(5000))//如果5s内有消息收到
    {
        //验证消息的正确性
        recvmsg=tcpsocket->readAll();while (tcpsocket->waitForReadyRead(200)) recvmsg+=tcpsocket->readAll();qDebug()<<"recv bytes "<<recvmsg.length();
        if (recvmsg.length()<36)//收到消息不满36字节
            return -12;
        recvlen=QByteArraytoInt(recvmsg.mid(0,4));
        if (recvmsg.length()<36+recvlen)//收到消息长度不对
            return -12;
        recvcontent=recvmsg.mid(36,recvlen);
        if (QCryptographicHash::hash(recvcontent,QCryptographicHash::Md5).toHex()!=recvmsg.mid(4,32))//收到消息md5不对
            return -12;
        qDebug()<<"recv: "<<recvcontent;
        //确认消息含义
        if (recvcontent.mid(0,strlen("request:pardon"))=="request:pardon")//如果服务器要求重发
            return 0;
        if (recvcontent.mid(0,strlen("response:logout"))=="response:logout")//如果服务器正常回应
            return 1;
        if (recvcontent=="response:pleaselogin")//如果服务器回应请登录
        {relogin();{relogin();return 2;}}
        return 10;
    }
    else
        return -11;
}

int MainWindow::tcp_initialstoc_head()
{
    QByteArray sendmsg="";//发送的完整消息
    QByteArray recvmsg="";//收到的完整消息
    QByteArray sendcontent="";//发送的正文
    QByteArray recvcontent="";//收到的正文
    int sendlen;
    int recvlen;

    if (tcpsocket->isWritable())//如果允许发消息
    {
        sendcontent+="request:initialstoc";
        sendlen=sendcontent.length();
        sendmsg+=InttoQByteArray(sendlen);
        sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
        sendmsg+=sendcontent;
        tcpsocket->write(sendmsg);
        qDebug()<<"send: "<<sendcontent;
    }
    else
        return -10;
    if (tcpsocket->waitForReadyRead(5000))//如果5s内有消息收到
    {
        //验证消息的正确性
        recvmsg=tcpsocket->readAll();while (tcpsocket->waitForReadyRead(200)) recvmsg+=tcpsocket->readAll();qDebug()<<"recv bytes "<<recvmsg.length();
        if (recvmsg.length()<36)//收到消息不满36字节
            return -12;
        recvlen=QByteArraytoInt(recvmsg.mid(0,4));
        if (recvmsg.length()<36+recvlen)//收到消息长度不对
            return -12;
        recvcontent=recvmsg.mid(36,recvlen);
        if (QCryptographicHash::hash(recvcontent,QCryptographicHash::Md5).toHex()!=recvmsg.mid(4,32))//收到消息md5不对
            return -12;
        qDebug()<<"recv: "<<recvcontent;
        //确认消息含义
        if (recvcontent.mid(0,strlen("request:pardon"))=="request:pardon")//如果服务器要求重发
            return 0;
        if (recvcontent.mid(0,strlen("response:initialstoc"))=="response:initialstoc")//如果服务器正常回应
        {
            //bool ok;
            int code=toint(recvcontent.mid(int(strlen("response:initialstoc,code:")),10));
            int pos=recvcontent.indexOf("timenow:");
            qDebug()<<"get time from server"<<recvcontent.mid(pos+int(strlen("timenow:")),19);

            if (pos>=0)
            {
                QString time=recvcontent.mid(pos+int(strlen("timenow:")),19);
                stocupdatetime=QDateTime::fromString (time,"yyyy-MM-dd HH:mm:ss");
            }

            //if (ok)
            return code;
            //else return 10;
        }
        if (recvcontent=="response:pleaselogin")//如果服务器回应请登录
        {relogin();return 2;}
        return 10;
    }
    else
        return -11;
}

int MainWindow::tcp_initialstoc_dir(int num)
{
    QByteArray sendmsg="";//发送的完整消息
    QByteArray recvmsg="";//收到的完整消息
    QByteArray sendcontent="";//发送的正文
    QByteArray recvcontent="";//收到的正文
    int sendlen;
    int recvlen;

    if (tcpsocket->isWritable())//如果允许发消息
    {
        sendcontent+="request:directory,num:";
        sendcontent+=QString::number(num,10);
        sendlen=sendcontent.length();
        sendmsg+=InttoQByteArray(sendlen);
        sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
        sendmsg+=sendcontent;
        tcpsocket->write(sendmsg);
        qDebug()<<"send: "<<sendcontent;
    }
    else
        return -10;
    if (tcpsocket->waitForReadyRead(5000))//如果5s内有消息收到
    {
        //验证消息的正确性
        recvmsg=tcpsocket->readAll();while (tcpsocket->waitForReadyRead(200)) recvmsg+=tcpsocket->readAll();qDebug()<<"recv bytes "<<recvmsg.length();
        //qDebug()<<"total len: "<<recvmsg.length();
        if (recvmsg.length()<36)//收到消息不满36字节
            return -12;
        recvlen=QByteArraytoInt(recvmsg.mid(0,4));
        if (recvmsg.length()<36+recvlen)//收到消息长度不对
            return -12;
        recvcontent=recvmsg.mid(36,recvlen);
        if (QCryptographicHash::hash(recvcontent,QCryptographicHash::Md5).toHex()!=recvmsg.mid(4,32))//收到消息md5不对
            return -12;
        qDebug()<<"recv: "<<recvcontent;
        //确认消息含义
        if (recvcontent.mid(0,strlen("request:pardon"))=="request:pardon")//如果服务器要求重发
            return 0;
        if (recvcontent.mid(0,strlen("response:directoryover"))=="response:directoryover")//如果服务器回应没有目录可发了,检查num是否正确
        {
            //bool ok;
            int recvnum=toint(recvcontent.mid(int(strlen("response:directoryover,num:")),10));
            if (recvnum==num)//可以转换并且num正确，判断为目录确实发完了
                return -1;
            //else if (!ok)//无法转换成数字，判断是无法解析的回应
            //return 10;
            else return 0;//num不正确 判断为丢包重发
        }
        if (recvcontent.mid(0,strlen("response:directory"))=="response:directory")//如果服务器正常回应
        {
            //bool ok;
            int recvnum=toint(recvcontent.mid(int(strlen("response:directory,num:")),10));
            //if (!ok)//无法转换成数字，判断是无法解析的回应
            //    return 10;
            if (recvnum!=num)//num不正确 判断为丢包重发
                return 0;
            //response:directory,num:{X},relativepathlen:{X},relativepath:{relativepath}
            int pos1=recvcontent.indexOf(",relativepathlen:");
            int pos2=recvcontent.indexOf(",relativepath:");
            if (pos1<0||pos2<0)//没有这两段字符就判断为无法解析的回应
                return 10;
            int relativepathlen=toint(recvcontent.mid(pos1+int(strlen(",relativepathlen:")),10));
            //if (!ok)//无法转换成数字，判断是无法解析的回应
            //    return 10;

            QString relativepath=recvcontent.mid(pos2+int(strlen(",relativepath:")),relativepathlen);
            dir.setPath(".");
            if (!dir.cd(relativepath))
                dir.mkpath(relativepath);
            return 1;
        }
        if (recvcontent=="response:pleaselogin")//如果服务器回应请登录
        {relogin();return 2;}
        return 10;
    }
    else
        return -11;
}

int MainWindow::tcp_initialstoc_file(int num)
{
    QByteArray sendmsg="";//发送的完整消息
    QByteArray recvmsg="";//收到的完整消息
    QByteArray sendcontent="";//发送的正文
    QByteArray recvcontent="";//收到的正文
    int sendlen;
    int recvlen;

    if (tcpsocket->isWritable())//如果允许发消息
    {
        sendcontent+="request:file,num:";
        sendcontent+=QString::number(num,10);
        sendlen=sendcontent.length();
        sendmsg+=InttoQByteArray(sendlen);
        sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
        sendmsg+=sendcontent;
        tcpsocket->write(sendmsg);
        qDebug()<<"send: "<<sendcontent;
    }
    else
        return -10;
    if (tcpsocket->waitForReadyRead(5000))//如果5s内有消息收到
    {
        //验证消息的正确性
        recvmsg=tcpsocket->readAll();
        while (tcpsocket->waitForReadyRead(200))
        {
            recvmsg+=tcpsocket->readAll();
            qDebug()<<"recv bytes "<<recvmsg.length();
        }
        if (recvmsg.length()<36)//收到消息不满36字节
            return -12;
        recvlen=QByteArraytoInt(recvmsg.mid(0,4));
        if (recvmsg.length()<36+recvlen)//收到消息长度不对
            return -12;
        recvcontent=recvmsg.mid(36,recvlen);
        if (QCryptographicHash::hash(recvcontent,QCryptographicHash::Md5).toHex()!=recvmsg.mid(4,32))//收到消息md5不对
            return -12;
        qDebug()<<"recv: "<<recvcontent;
        //确认消息含义
        if (recvcontent.mid(0,strlen("request:pardon"))=="request:pardon")//如果服务器要求重发
            return 0;
        if (recvcontent.mid(0,strlen("response:fileover"))=="response:fileover")//如果服务器回应没有文件可发了,检查num是否正确
        {
            //bool ok;
            int recvnum=toint(recvcontent.mid(int(strlen("response:fileover,num:")),10));
            if (recvnum==num)//可以转换并且num正确，判断为目录确实发完了
                return -1;
            // else if (!ok)//无法转换成数字，判断是无法解析的回应
            //   return 10;
            else return 0;//num不正确 判断为丢包重发
        }
        if (recvcontent.mid(0,strlen("response:file"))=="response:file")//如果服务器正常回应
        {
            //bool ok;
            int recvnum=toint(recvcontent.mid(int(strlen("response:file,num:")),10));
            //if (!ok)//无法转换成数字，判断是无法解析的回应
            //   return 10;
            if (recvnum!=num)//num不正确 判断为丢包重发
                return 0;
            //response:file,num:{X},relativepathlen:{X},relativepath:{relativepath},filenamelen:{X},filename:{filename},file:{MD5}
            int pos1=recvcontent.indexOf(",relativepathlen:");
            int pos2=recvcontent.indexOf(",relativepath:");
            int pos3=recvcontent.indexOf(",filenamelen:");
            int pos4=recvcontent.indexOf(",filename:");
            int pos5=recvcontent.indexOf(",file:");
            int pos6=recvcontent.indexOf(",total:");
            if (pos1<0||pos2<0||pos3<0||pos4<0||pos5<0||pos6<0)//没有这6段字符就判断为无法解析的回应
                return 10;
            int relativepathlen=toint(recvcontent.mid(pos1+int(strlen(",relativepathlen:")),10));
            //if (!ok)//无法转换成数字，判断是无法解析的回应
            //     return 10;
            int filenamelen=toint(recvcontent.mid(pos3+int(strlen(",filenamelen:")),10));
            //if (!ok)//无法转换成数字，判断是无法解析的回应
            //    return 10;
            int total=toint(recvcontent.mid(pos6+int(strlen(",total:")),10));
            QString relativepath=recvcontent.mid(pos2+int(strlen(",relativepath:")),relativepathlen);
            QString filename=recvcontent.mid(pos4+int(strlen(",filename:")),filenamelen);
            dir.setPath(".");
            if (!dir.cd(relativepath)){
                dir.mkpath(relativepath);
                dir.cd(relativepath);
            }
            QByteArray smd5=recvcontent.mid(pos5+int(strlen(",file:")),32);
            QByteArray cmd5;
            dir.cd(".");
            QString filepath=dir.filePath(filename);
            qDebug()<<"check file:"<<filepath;
            QFile*   newfile=new QFile(filepath);
            if (newfile->exists())//如果同名文件存在
            {
                if (newfile->open(QIODevice::ReadOnly))
                {
                    QByteArray content=newfile->readAll();
                    newfile->close();
                    cmd5 = QCryptographicHash::hash(content,QCryptographicHash::Md5).toHex();
                }
                newfile->close();
                if (smd5==cmd5)
                {
                    return 1;
                }
                else {

                    QString newname=newfile->fileName()+".old";
                    while (!newfile->rename(newname))
                        newname+=".old";
                    newfile=new QFile(filepath);
                    on_recvfile(newfile,smd5,total);
                }
            }
            else
            {
                on_recvfile(newfile,smd5,total);
            }
            return 1;
        }
        if (recvcontent=="response:pleaselogin")//如果服务器回应请登录
        {relogin();return 2;}
        return 10;
    }
    else
        return -11;
}

void MainWindow::on_recvfile(QFile*file,QByteArray md5,int total)
{
    int i;
    for (i=0;i<tempfileList.size();i++)
    {
        QFile tempfile(tempfileList.at(i));
        if (!tempfile.exists()){//路径不存在的不要
            tempfileList.removeAt(i--);
            continue;
        }
        if (!tempfile.open(QIODevice::ReadOnly ))//打不开的不要
            continue;
        if (tempfile.fileName().mid(32,32)!=md5)//文件名后三十二位不等于完整文件md5的不要
            continue;
        QString tmd5=QCryptographicHash::hash(tempfile.readAll(),QCryptographicHash::Md5).toHex();
        if (tempfile.fileName().mid(0,32)!=tmd5){//文件与md5不符合的不要
            if(tempfile.fileName().lastIndexOf(".sjtempfile")+int(strlen(".sjtempfile")==tempfile.fileName().length()))//失效的临时文件
                QFile::remove(QFileInfo(tempfile).absoluteFilePath());
            tempfileList.removeAt(i--);
            continue;
        }
        else//有符合条件的临时文件
        {
            if (file->exists())
                QFile::remove(QFileInfo(*file).absoluteFilePath());
            QFile::copy(tempfileList.at(i),QFileInfo(*file).absoluteFilePath());
            QFile::remove(tempfileList.at(i));//删除临时文件
            if (!file->open(QIODevice::ReadWrite))
                return;
            QFileInfo info;
            info.setFile(*file);
            transportdialog->reset_downfile(info.absoluteFilePath(),file->fileName(),total);
            QByteArray content;
            int size=int(file->size()),len,res;
            while (true)
            {
resend:
                res=tcp_recv_file(md5,size,len,content);
                if (res==0||res==-12||res==10)
                    goto resend;
                else if (res==-10||res==-11)//连接中断，当成临时文件处理
                {
                    QString tempfilename=QCryptographicHash::hash(file->readAll(),QCryptographicHash::Md5).toHex();
                    file->close();
                    transportdialog->reset_downfile("","",0);
                    tempfilename+=md5;
                    tempfilename+=".sjtempfile";
                    file->rename(tempfilename);
                    tempfileList.append(QFileInfo(*file).absoluteFilePath());
                    return;
                }
                size+=len;
                transportdialog->reset_downnow(size);
                if (len<=0)
                    break;
                file->write(content,len);
            }
            file->close();
            transportdialog->reset_downfile("","",0);
            if (userlog&&userlog->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
            {
                QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
                QString timestr = time.toString("yyyy-MM-dd HH:mm:ss: "); //设置显示格式
                timestr+="received from breakpoint,file:"+QFileInfo(*file).absoluteFilePath();
                userlog->write(timestr.toLatin1());
                userlog->close();
            }
            return;
        }

    }

    if (!file->open(QIODevice::ReadWrite|QIODevice::Truncate))
        return;
    QFileInfo info;
    info.setFile(*file);
    transportdialog->reset_downfile(info.absoluteFilePath(),file->fileName(),total);
    QByteArray content;
    int size=0,len,res;
    while (true)
    {
resend1:
        res=tcp_recv_file(md5,size,len,content);
        if (res==0||res==-12||res==10)
            goto resend1;
        else if (res==-10||res==-11)//连接中断，当成临时文件处理
        {
            QString tempfilename=QCryptographicHash::hash(file->readAll(),QCryptographicHash::Md5).toHex();
            file->close();
            transportdialog->reset_downfile("","",0);
            tempfilename+=md5;
            tempfilename+=".sjtempfile";
            file->rename(tempfilename);
            tempfileList.append(QFileInfo(*file).absoluteFilePath());
            return;
        }
        size+=len;
        transportdialog->reset_downnow(size);
        if (len<=0)
            break;
        file->write(content,len);
    }
    file->close();
    transportdialog->reset_downfile("","",0);
    if (userlog&&userlog->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
    {
        QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
        QString timestr = time.toString("yyyy-MM-dd HH:mm:ss: "); //设置显示格式
        timestr+="received a file,file:"+QFileInfo(*file).absoluteFilePath();
        userlog->write(timestr.toLatin1());
        userlog->close();
    }
}

int MainWindow::tcp_recv_file(QByteArray md5,int startfrom,int& len,QByteArray&content)
{
    QByteArray sendmsg="";//发送的完整消息
    QByteArray recvmsg="";//收到的完整消息
    QByteArray sendcontent="";//发送的正文
    QByteArray recvcontent="";//收到的正文
    int sendlen;
    int recvlen;

    if (tcpsocket->isWritable())//如果允许发消息
    {
        sendcontent+="request:recvfile,file:";
        sendcontent+=md5;
        sendcontent+=",startfrom:";
        sendcontent+=QString::number(startfrom,10);

        sendlen=sendcontent.length();
        sendmsg+=InttoQByteArray(sendlen);
        sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
        sendmsg+=sendcontent;
        tcpsocket->write(sendmsg);
        qDebug()<<"send: "<<sendcontent;
    }
    else
        return -10;
    if (tcpsocket->waitForReadyRead(5000))//如果5s内有消息收到
    {
        //验证消息的正确性
        recvmsg=tcpsocket->readAll();
        while (tcpsocket->waitForReadyRead(200))
            recvmsg+=tcpsocket->readAll();
        qDebug()<<"recv bytes "<<recvmsg.length();
        if (recvmsg.length()<36)//收到消息不满36字节
            return -12;
        recvlen=QByteArraytoInt(recvmsg.mid(0,4));
        qDebug()<<"recvlen="<<recvlen;
        if (recvmsg.length()<36+recvlen)//收到消息长度不对
            return -12;
        recvcontent=recvmsg.mid(36,recvlen);
        if (QCryptographicHash::hash(recvcontent,QCryptographicHash::Md5).toHex()!=recvmsg.mid(4,32))//收到消息md5不对
        {
            int pos4=recvcontent.indexOf(",content:");
            int pos3=recvcontent.indexOf(",len:");
            len=toint(recvcontent.mid(pos3+int(strlen(",len:")),10));
            content=recvcontent.mid(pos4+int(strlen(",content:")),len);
            QFile file("./test.txt");
            file.open(QIODevice::Truncate|QIODevice::WriteOnly);
            file.write(content);
            //exit(0);
            return -12;
        }
        //qDebug()<<"recv file..";
        qDebug()<<"recv: "<<recvcontent;
        //确认消息含义
        if (recvcontent.mid(0,strlen("request:pardon"))=="request:pardon")//如果服务器要求重发
            return 0;
        if (recvcontent.mid(0,strlen("response:sendfile"))=="response:sendfile")//如果服务器正常回应
        {
            //bool ok;
            //response:sendfile,file:{MD5},startfrom:{X},len:{X},content:{filestream}
            int pos1=recvcontent.indexOf(",file:");
            int pos2=recvcontent.indexOf(",startfrom:");
            int pos3=recvcontent.indexOf(",len:");
            int pos4=recvcontent.indexOf(",content:");
            if (pos1<0||pos2<0||pos3<0||pos4<0)//没有这四段字符就判断为无法解析的回应
                return 10;
            QByteArray recvmd5=recvcontent.mid(pos1+int(strlen(",file:")),32);
            int recvstartfrom=toint(recvcontent.mid(pos2+int(strlen(",startfrom:")),10));
            // if (!ok)//无法转换成数字，判断是无法解析的回应
            //   return 10;
            len=toint(recvcontent.mid(pos3+int(strlen(",len:")),10));
            //if (!ok)//无法转换成数字，判断是无法解析的回应
            //    return 10;
            if (recvmd5!=md5||startfrom!=recvstartfrom)//md5或者startfrom不对，判断丢包重发
                return 0;
            content=recvcontent.mid(pos4+int(strlen(",content:")),len);
            return 1;
        }
        if (recvcontent=="response:pleaselogin")//如果服务器回应请登录
        {relogin();return 2;}
        return 10;
    }
    else
        return -11;
}

int MainWindow::tcp_initialctos_head()
{
    QByteArray sendmsg="";//发送的完整消息
    QByteArray recvmsg="";//收到的完整消息
    QByteArray sendcontent="";//发送的正文
    QByteArray recvcontent="";//收到的正文
    int sendlen;
    int recvlen;

    if (tcpsocket->isWritable())//如果允许发消息
    {
        sendcontent+="request:initialctos";
        sendlen=sendcontent.length();
        sendmsg+=InttoQByteArray(sendlen);
        sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
        sendmsg+=sendcontent;
        tcpsocket->write(sendmsg);
        qDebug()<<"send: "<<sendcontent;
    }
    else
        return -10;
    if (tcpsocket->waitForReadyRead(5000))//如果5s内有消息收到
    {
        //验证消息的正确性
        recvmsg=tcpsocket->readAll();while (tcpsocket->waitForReadyRead(200)) recvmsg+=tcpsocket->readAll();qDebug()<<"recv bytes "<<recvmsg.length();
        if (recvmsg.length()<36)//收到消息不满36字节
            return -12;
        recvlen=QByteArraytoInt(recvmsg.mid(0,4));
        if (recvmsg.length()<36+recvlen)//收到消息长度不对
            return -12;
        recvcontent=recvmsg.mid(36,recvlen);
        if (QCryptographicHash::hash(recvcontent,QCryptographicHash::Md5).toHex()!=recvmsg.mid(4,32))//收到消息md5不对
            return -12;
        qDebug()<<"recv: "<<recvcontent;
        //确认消息含义
        if (recvcontent.mid(0,strlen("request:pardon"))=="request:pardon")//如果服务器要求重发
            return 0;
        if (recvcontent.mid(0,strlen("response:initialctos"))=="response:initialctos")//如果服务器正常回应
        {
            // bool ok;
            int code=toint(recvcontent.mid(int(strlen("response:initialctos,code:")),10));
            //if (ok)
            return code;
            //else return 10;
        }
        if (recvcontent=="response:pleaselogin")//如果服务器回应请登录
        {relogin();return 2;}
        return 10;
    }
    else
        return -11;
}

int MainWindow::tcp_initialstoc()
{
    if (tcp_initialstoc_head()==-1)
        return -1;
    int i=0;
    int res;
    while (true)
    {
        res=tcp_initialstoc_dir(i++);
        if (res==-1){
            qDebug()<<"init recv dir over,start to recvfile"<<res;
            break;
        }
    }
    i=0;
    while (true)
    {
        res=tcp_initialstoc_file(i++);
        if (res==-1)
        {
            qDebug()<<"init recv file over"<<res;
            break;
        }
    }
    return 0;
}

int MainWindow::tcp_initialctos()
{
    tcp_initialctos_head();
    QFileInfoList filelist,dirlist;
    dir.setPath(".");
    filelist.clear();
    dirlist.clear();
    dirlist.append(QFileInfo("."));
    addlist(dir,dirlist,filelist);
    tcp_initialctos_all(dirlist,filelist);
    return 0;
}

int MainWindow::tcp_initialctos_all(QFileInfoList&dirlist,QFileInfoList&filelist)
{
    QByteArray sendmsg="";//发送的完整消息
    QByteArray recvmsg="";//收到的完整消息
    QByteArray sendcontent="";//发送的正文
    QByteArray recvcontent="";//收到的正文
    int sendlen;
    int recvlen;
    QFile *file=new QFile();
    QByteArray md5,allcontent;
    int len,startfrom;
    if (tcpsocket->isWritable())//如果允许发消息
    {
        sendcontent+="request:senddirectory";

        sendlen=sendcontent.length();
        sendmsg+=InttoQByteArray(sendlen);
        sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
        sendmsg+=sendcontent;
        tcpsocket->write(sendmsg);
        qDebug()<<"send: "<<sendcontent;
    }
    else
        return -10;
receive:
    if (tcpsocket->waitForReadyRead(5000))//如果5s内有消息收到
    {
        //验证消息的正确性
        recvmsg=tcpsocket->readAll();while (tcpsocket->waitForReadyRead(200)) recvmsg+=tcpsocket->readAll();
        //qDebug()<<"recv bytes "<<recvmsg.length();
        if (recvmsg.length()<36)//收到消息不满36字节
            return -12;
        recvlen=QByteArraytoInt(recvmsg.mid(0,4));
        if (recvmsg.length()<36+recvlen)//收到消息长度不对
            return -12;
        recvcontent=recvmsg.mid(36,recvlen);
        if (QCryptographicHash::hash(recvcontent,QCryptographicHash::Md5).toHex()!=recvmsg.mid(4,32))//收到消息md5不对
            return -12;
        qDebug()<<"recv "<<recvcontent;
        //确认消息含义
        if (recvcontent.mid(0,strlen("request:pardon"))=="request:pardon")//如果服务器要求重发
            return 0;
        else if (recvcontent.mid(0,strlen("request:directory"))=="request:directory")//如果服务器请求目录
        {
            //bool ok;
            int num=toint(recvcontent.mid(recvcontent.indexOf(",num:")+int(strlen(",num:")),10));
            // if (!ok)
            //   return 10;
            if (num>=dirlist.size()||num<0)//如果目录发完了
            {
                QByteArray sendmsg="";//发送的完整消息
                QByteArray sendcontent="";//发送的正文
                if (tcpsocket->isWritable())//如果允许发消息
                {
                    sendcontent+="response:directoryover,num:";
                    sendcontent+=QString::number(num,10);

                    sendlen=sendcontent.length();
                    sendmsg+=InttoQByteArray(sendlen);
                    sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
                    sendmsg+=sendcontent;
                    tcpsocket->write(sendmsg);
                    qDebug()<<"send: "<<sendcontent;
                    goto receive;
                }
                else
                    return -10;
            }
            else
            {
                QByteArray sendmsg="";//发送的完整消息
                QByteArray sendcontent="";//发送的正文
                if (tcpsocket->isWritable())//如果允许发消息
                {
                    QFileInfo dirinfo=dirlist.at(num);
                    QString relativepath=dirinfo.filePath();
                    sendcontent+="response:directory,num:";
                    sendcontent+=QString::number(num,10);
                    sendcontent+=",relativepathlen:";
                    sendcontent+=QString::number(relativepath.length(),10);
                    sendcontent+=",relativepath:";
                    sendcontent+=relativepath;

                    sendlen=sendcontent.length();
                    sendmsg+=InttoQByteArray(sendlen);
                    sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
                    sendmsg+=sendcontent;
                    tcpsocket->write(sendmsg);
                    qDebug()<<"send: "<<sendcontent;
                    goto receive;
                }
                else
                    return -10;
            }

        }
        else if (recvcontent.mid(0,strlen("request:file"))=="request:file")//如果服务器请求文件
        {
            //bool ok;
            int num=toint(recvcontent.mid(recvcontent.indexOf(",num:")+int(strlen(",num:")),10));
            // if (!ok)
            //     return 10;
            if (num>=filelist.size()||num<0)//如果文件发完了
            {
                QByteArray sendmsg="";//发送的完整消息
                QByteArray sendcontent="";//发送的正文
                if (tcpsocket->isWritable())//如果允许发消息
                {
                    sendcontent+="response:fileover,num:";
                    sendcontent+=QString::number(num,10);

                    sendlen=sendcontent.length();
                    sendmsg+=InttoQByteArray(sendlen);
                    sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
                    sendmsg+=sendcontent;
                    tcpsocket->write(sendmsg);
                    qDebug()<<"send: "<<sendcontent;
                    return 1;
                }
                else
                    return -10;
            }
            else
            {
                QByteArray sendmsg="";//发送的完整消息
                QByteArray sendcontent="";//发送的正文
                if (tcpsocket->isWritable())//如果允许发消息
                {
                    QFileInfo fileinfo=filelist.at(num);
                    QString relativepath=fileinfo.path();
                    QString filename=fileinfo.fileName();
                    file=new QFile(fileinfo.filePath());
                    file->open(QIODevice::ReadOnly);
                    allcontent=file->readAll();
                    md5=QCryptographicHash::hash(allcontent,QCryptographicHash::Md5).toHex();
                    file->close();
                    sendcontent+="response:file,num:";
                    sendcontent+=QString::number(num,10);
                    sendcontent+=",relativepathlen:";
                    sendcontent+=QString::number(relativepath.length(),10);
                    sendcontent+=",relativepath:";
                    sendcontent+=relativepath;

                    sendcontent+=",filenamelen:";
                    sendcontent+=QString::number(filename.length(),10);
                    sendcontent+=",filename:";
                    sendcontent+=filename;

                    sendcontent+=",file:";
                    sendcontent+=md5;

                    sendlen=sendcontent.length();
                    sendmsg+=InttoQByteArray(sendlen);
                    sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
                    sendmsg+=sendcontent;
                    tcpsocket->write(sendmsg);
                    qDebug()<<"send: "<<sendcontent;
                    goto receive;
                }
                else
                    return -10;
            }

        }
        else if (recvcontent.mid(0,strlen("request:skipfile"))=="request:skipfile")//如果服务器回应跳过
        {
            //bool ok;
            int code=toint(recvcontent.mid(recvcontent.indexOf(",code:")+int(strlen(",code:")),10));
            //if (!ok)
            //    return 10;
            int num=toint(recvcontent.mid(recvcontent.indexOf(",num:")+int(strlen(",num:")),10));
            // if (!ok)
            //    return 10;
            if (code==1)//如果服务器秒传
            {
                QString filepath=filelist.at(num).filePath();
                if (userlog&&userlog->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
                {
                    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
                    QString timestr = time.toString("yyyy-MM-dd HH:mm:ss: "); //设置显示格式
                    timestr+="file: ";
                    timestr+=filepath;
                    timestr+=" send in a second\n";
                    userlog->write(timestr.toLatin1());
                    userlog->close();
                }
            }
            QByteArray sendmsg="";//发送的完整消息
            QByteArray sendcontent="";//发送的正文
            if (tcpsocket->isWritable())//如果允许发消息
            {
                sendcontent+="response:nextfile,num:";
                sendcontent+=QString::number(num,10);

                sendlen=sendcontent.length();
                sendmsg+=InttoQByteArray(sendlen);
                sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
                sendmsg+=sendcontent;
                tcpsocket->write(sendmsg);
                qDebug()<<"send: "<<sendcontent;
                goto receive;
            }
            else
                return -10;
        }
        else if (recvcontent.mid(0,strlen("request:recvfile"))=="request:recvfile")//如果服务器请求接受文件
        {
            //bool ok;
            //response:sendfile,file:{MD5},startfrom:{X},len:{X},content:{filestream}
            int pos1=recvcontent.indexOf(",relativepathlen:");
            int pos2=recvcontent.indexOf(",relativepath:");
            int pos3=recvcontent.indexOf(",filenamelen:");
            int pos4=recvcontent.indexOf(",filename:");
            int pos5=recvcontent.indexOf(",file:");
            int pos6=recvcontent.indexOf(",startfrom:");
            if (pos1<0||pos2<0||pos3<0||pos4<0||pos5<0||pos6<0)//没有这6段字符就判断为无法解析的回应
                return 10;
            int relativepathlen=toint(recvcontent.mid(pos1+int(strlen(",relativepathlen:")),10));
            //qDebug()<<recvcontent.mid(pos1+int(strlen(",relativepathlen:")),10);
            //if (!ok)//无法转换成数字，判断是无法解析的回应
            //    return 10;
            int filenamelen=toint(recvcontent.mid(pos3+int(strlen(",filenamelen:")),10));
            //if (!ok)//无法转换成数字，判断是无法解析的回应
            //    return 10;
            startfrom=toint(recvcontent.mid(pos6+int(strlen(",startfrom:")),10));
            //if (!ok)//无法转换成数字，判断是无法解析的回应
            //    return 10;
            QString relativepath=recvcontent.mid(pos2+int(strlen(",relativepath:")),relativepathlen);
            QString filename=recvcontent.mid(pos4+int(strlen(",filename:")),filenamelen);
            QByteArray newmd5=recvcontent.mid(pos5+int(strlen(",file:")),32);
            if (newmd5!=md5)
            {
                md5=newmd5;
                dir.setPath(".");
                dir.cd(relativepath);
                QString filepath=dir.filePath(filename);
                file=new QFile(filepath);
                file->open(QIODevice::ReadOnly);
                allcontent=file->readAll();
                file->close();
            }
            QFileInfo info;
            info.setFile(*file);
            transportdialog->reset_upfile(info.absoluteFilePath(),file->fileName(),int(info.size()));

            if (tcpsocket->isWritable())//如果允许发消息
            {
                len=allcontent.length()-startfrom;
                if (len<0)
                    len=0;
                else if (len>BLOCK_SIZE)
                    len=BLOCK_SIZE;
                sendmsg="";
                sendcontent="";
                sendcontent+="response:sendfile,relativepathlen:";
                sendcontent+=QString::number(relativepathlen,10);
                sendcontent+=",relativepath:";
                sendcontent+=relativepath;
                sendcontent+=",filenamelen:";
                sendcontent+=QString::number(filenamelen,10);
                sendcontent+=",filename:";
                sendcontent+=filename;
                sendcontent+=",file:";
                sendcontent+=md5;
                sendcontent+=",len:";
                sendcontent+=QString::number(len,10);
                sendcontent+=",content:";
                sendcontent+=allcontent.mid(startfrom,len);
                sendlen=sendcontent.length();
                sendmsg+=InttoQByteArray(sendlen);
                sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
                sendmsg+=sendcontent;
                tcpsocket->write(sendmsg);
                transportdialog->reset_upnow(startfrom+len);
                if (len==0)
                    transportdialog->reset_upfile("","",0);
                qDebug()<<"send file";
                //qDebug()<<"send: "<<sendcontent;
                goto receive;
            }
            else
                return -10;

        }
        else if (recvcontent=="response:pleaselogin")//如果服务器回应请登录
        {relogin();return 2;}
        return 10;

    }
    return -11;
}

int MainWindow::tcp_updatestoc()
{
    tcp_updatestoc_head();
    dir.cd(".");
    tcp_updatestoc_all(dir);
    return 0;
}

int MainWindow::tcp_updatestoc_head()
{
    QByteArray sendmsg="";//发送的完整消息
    QByteArray recvmsg="";//收到的完整消息
    QByteArray sendcontent="";//发送的正文
    QByteArray recvcontent="";//收到的正文
    int sendlen;
    int recvlen;

    if (tcpsocket->isWritable())//如果允许发消息
    {
        sendcontent+="request:updatestoc,timeafter:";
        sendcontent+=stocupdatetime.toString("yyyy-MM-dd HH:mm:ss");

        sendlen=sendcontent.length();
        sendmsg+=InttoQByteArray(sendlen);
        sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
        sendmsg+=sendcontent;
        tcpsocket->write(sendmsg);
        qDebug()<<"send: "<<sendcontent;
    }
    else
        return -10;
    if (tcpsocket->waitForReadyRead(5000))//如果5s内有消息收到
    {
        //验证消息的正确性
        recvmsg=tcpsocket->readAll();while (tcpsocket->waitForReadyRead(200)) recvmsg+=tcpsocket->readAll();
        qDebug()<<"recv bytes "<<recvmsg.length();
        if (recvmsg.length()<36)//收到消息不满36字节
            return -12;
        recvlen=QByteArraytoInt(recvmsg.mid(0,4));
        qDebug()<<"recvlen"<<recvlen;
        if (recvmsg.length()<36+recvlen)//收到消息长度不对
            return -12;
        recvcontent=recvmsg.mid(36,recvlen);
        if (QCryptographicHash::hash(recvcontent,QCryptographicHash::Md5).toHex()!=recvmsg.mid(4,32))//收到消息md5不对
            return -12;
        qDebug()<<"recv: "<<recvcontent;
        //确认消息含义
        if (recvcontent.mid(0,strlen("request:pardon"))=="request:pardon")//如果服务器要求重发
            return 0;
        if (recvcontent.mid(0,strlen("response:updatestoc"))=="response:updatestoc")//如果服务器正常回应
        {
            // bool ok;
            int code=toint(recvcontent.mid(int(strlen("response:updatestoc,code:")),10));
            stocupdatetime=QDateTime::fromString(recvcontent.mid(recvcontent.indexOf(",timenow:")+int(strlen(",timenow:")),19),"yyyy-MM-dd HH:mm:ss");
            //if (ok)
            return code;
            //else return 10;
        }
        if (recvcontent=="response:pleaselogin")//如果服务器回应请登录
        {relogin();return 2;}
        return 10;
    }
    else
        return -11;
}

int MainWindow::tcp_updatestoc_all(QDir dir)
{
    if (!dir.exists())
        return -1;
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
            int res=tcp_updatestoc_dir(fileInfo.filePath());
            if (res==-1)
            {
                dir.remove(fileInfo.filePath());
            }
            else if (res==1)
            {
                tcp_updatestoc_all(QDir(fileInfo.filePath()));
            }
        }
        else
        {
            if (fileInfo.size()!=0&&fileInfo.fileName().mid(0,2)!="~$"&&fileInfo.suffix()!="sjtempfile"&&fileInfo.size()<MAX_FILE_SIZE)
            {
                int res=tcp_updatestoc_dir(fileInfo.filePath());
                if (res==-1)
                {
                    dir.remove(fileInfo.filePath());
                }
            }

        }
        i++;
    }
    while(i<list.size());
    i=0;
    while (true)
    {
        if (tcp_updatestoc_newdir(QFileInfo(dir.path()),i++)==-1||tcp_updatestoc_newfile(QFileInfo(dir.path()),i++)==-11)
            break;
    }
    i=0;
    while (true)
    {
        if (tcp_updatestoc_newfile(QFileInfo(dir.path()),i++)==-1||tcp_updatestoc_newfile(QFileInfo(dir.path()),i++)==-11)
            break;
    }
    return 1;
}

int MainWindow::tcp_updatestoc_dir(QString relativepath)
{
    QByteArray sendmsg="";//发送的完整消息
    QByteArray recvmsg="";//收到的完整消息
    QByteArray sendcontent="";//发送的正文
    QByteArray recvcontent="";//收到的正文
    int sendlen;
    int recvlen;

    if (tcpsocket->isWritable())//如果允许发消息
    {
        sendcontent+="request:dirupdateinfo,relativepathlen:";
        QString::number(relativepath.length(),10);
        sendcontent+=",relativepath:";
        sendcontent+=relativepath;

        sendlen=sendcontent.length();
        sendmsg+=InttoQByteArray(sendlen);
        sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
        sendmsg+=sendcontent;
        tcpsocket->write(sendmsg);
        qDebug()<<"send: "<<sendcontent;
    }
    else
        return -10;
    if (tcpsocket->waitForReadyRead(5000))//如果5s内有消息收到
    {
        //验证消息的正确性
        recvmsg=tcpsocket->readAll();while (tcpsocket->waitForReadyRead(200)) recvmsg+=tcpsocket->readAll();qDebug()<<"recv bytes "<<recvmsg.length();
        if (recvmsg.length()<36)//收到消息不满36字节
            return -12;
        recvlen=QByteArraytoInt(recvmsg.mid(0,4));
        if (recvmsg.length()<36+recvlen)//收到消息长度不对
            return -12;
        recvcontent=recvmsg.mid(36,recvlen);
        if (QCryptographicHash::hash(recvcontent,QCryptographicHash::Md5).toHex()!=recvmsg.mid(4,32))//收到消息md5不对
            return -12;
        qDebug()<<"recv: "<<recvcontent;
        //确认消息含义
        if (recvcontent.mid(0,strlen("request:pardon"))=="request:pardon")//如果服务器要求重发
            return 0;
        if (recvcontent.mid(0,strlen("response:dirupdateinfo"))=="response:dirupdateinfo")//如果服务器正常回应
        {
            // bool ok;
            int code=toint(recvcontent.mid(recvcontent.indexOf(",code:")+int(strlen(",code:")),10));
            return code;
        }
        if (recvcontent=="response:pleaselogin")//如果服务器回应请登录
        {relogin();return 2;}
        return 10;
    }
    else
        return -11;
}

int MainWindow::tcp_updatestoc_file(QFileInfo info)
{
    QByteArray sendmsg="";//发送的完整消息
    QByteArray recvmsg="";//收到的完整消息
    QByteArray sendcontent="";//发送的正文
    QByteArray recvcontent="";//收到的正文
    int sendlen;
    int recvlen;

    if (tcpsocket->isWritable())//如果允许发消息
    {
        QString relativepath=info.path();
        QString filename=info.fileName();
        sendcontent+="request:fileupdateinfo,relativepathlen:";
        sendcontent+=QString::number(relativepath.length(),10);
        sendcontent+=",relativepath:";
        sendcontent+=relativepath;
        sendcontent+=",filenamelen:";
        sendcontent+=QString::number(filename.length(),10);
        sendcontent+=",filename:";
        sendcontent+=filename;


        sendlen=sendcontent.length();
        sendmsg+=InttoQByteArray(sendlen);
        sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
        sendmsg+=sendcontent;
        tcpsocket->write(sendmsg);
        qDebug()<<"send: "<<sendcontent;
    }
    else
        return -10;
    if (tcpsocket->waitForReadyRead(5000))//如果5s内有消息收到
    {
        //验证消息的正确性
        recvmsg=tcpsocket->readAll();while (tcpsocket->waitForReadyRead(200)) recvmsg+=tcpsocket->readAll();qDebug()<<"recv bytes "<<recvmsg.length();
        if (recvmsg.length()<36)//收到消息不满36字节
            return -12;
        recvlen=QByteArraytoInt(recvmsg.mid(0,4));
        if (recvmsg.length()<36+recvlen)//收到消息长度不对
            return -12;
        recvcontent=recvmsg.mid(36,recvlen);
        if (QCryptographicHash::hash(recvcontent,QCryptographicHash::Md5).toHex()!=recvmsg.mid(4,32))//收到消息md5不对
            return -12;
        qDebug()<<"recv: "<<recvcontent;
        //确认消息含义
        if (recvcontent.mid(0,strlen("request:pardon"))=="request:pardon")//如果服务器要求重发
            return 0;
        if (recvcontent.mid(0,strlen("response:dirupdateinfo"))=="response:dirupdateinfo")//如果服务器正常回应
        {
            // bool ok;
            int code=toint(recvcontent.mid(recvcontent.indexOf(",code:")+int(strlen(",code:")),10));

            if (code==1)
            {
                int filenamelen=toint(recvcontent.mid(recvcontent.indexOf(",filenamelen:")+int(strlen(",filenamelen:")),10));
                QString filename=recvcontent.mid(recvcontent.indexOf(",filename:")+int(strlen(",filename:")),filenamelen);
                int pathlen=toint(recvcontent.mid(recvcontent.indexOf(",relativepathlen:")+int(strlen(",relativepathlen:")),10));
                QString path=recvcontent.mid(recvcontent.indexOf(",relativepath:")+int(strlen(",relativepath:")),pathlen);
                dir.cd(".");
                dir.cd(path);
                QString filepath=dir.filePath(filename);
                QFile* newfile=new QFile(filepath);
                QByteArray md5=recvcontent.mid(recvcontent.indexOf(",file:")+int(strlen(",file:")),32);
                int total=toint(recvcontent.mid(recvcontent.indexOf(",total:")+int(strlen(",total:")),10));
                on_recvfile(newfile,md5,total);
                return 1;
            }
            return code;
        }
        if (recvcontent=="response:pleaselogin")//如果服务器回应请登录
        {relogin();return 2;}
        return 10;
    }
    else
        return -11;
}

int MainWindow::tcp_updatestoc_newdir(QFileInfo info,int num)
{
    QByteArray sendmsg="";//发送的完整消息
    QByteArray recvmsg="";//收到的完整消息
    QByteArray sendcontent="";//发送的正文
    QByteArray recvcontent="";//收到的正文
    int sendlen;
    int recvlen;

    if (tcpsocket->isWritable())//如果允许发消息
    {
        QString relativepath=info.path();
        sendcontent+="request:newdir,relativepathlen:";
        sendcontent+=QString::number(relativepath.length(),10);
        sendcontent+=",relativepath:";
        sendcontent+=relativepath;
        sendcontent+=",num:";
        sendcontent+=QString::number(num,10);


        sendlen=sendcontent.length();
        sendmsg+=InttoQByteArray(sendlen);
        sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
        sendmsg+=sendcontent;
        tcpsocket->write(sendmsg);
        qDebug()<<"send: "<<sendcontent;
    }
    else
        return -10;
    if (tcpsocket->waitForReadyRead(5000))//如果5s内有消息收到
    {
        //验证消息的正确性
        recvmsg=tcpsocket->readAll();while (tcpsocket->waitForReadyRead(200)) recvmsg+=tcpsocket->readAll();qDebug()<<"recv bytes "<<recvmsg.length();
        if (recvmsg.length()<36)//收到消息不满36字节
            return -12;
        recvlen=QByteArraytoInt(recvmsg.mid(0,4));
        if (recvmsg.length()<36+recvlen)//收到消息长度不对
            return -12;
        recvcontent=recvmsg.mid(36,recvlen);
        if (QCryptographicHash::hash(recvcontent,QCryptographicHash::Md5).toHex()!=recvmsg.mid(4,32))//收到消息md5不对
            return -12;
        qDebug()<<"recv: "<<recvcontent;
        //确认消息含义
        if (recvcontent.mid(0,strlen("request:pardon"))=="request:pardon")//如果服务器要求重发
            return 0;
        if (recvcontent.mid(0,strlen("response:nonewdir"))=="response:nonewdir")//回应没有新目录了
        {
            return -1;
        }
        if (recvcontent.mid(0,strlen("response:newdir"))=="response:newdir")//如果服务器正常回应
        {
            // bool ok;
            int newrelativepathlen=toint(recvcontent.mid(recvcontent.indexOf(",newrelativepathlen:")+int(strlen(",newrelativepathlen:")),10));
            QString newdir=recvcontent.mid(recvcontent.indexOf(",newrelativepath:")+int(strlen(",newrelativepath:")),newrelativepathlen);
            dir.mkdir(newdir);
            return 1;
        }
        if (recvcontent=="response:pleaselogin")//如果服务器回应请登录
        {relogin();return 2;}
        return 10;
    }
    else
        return -11;
}

int MainWindow::tcp_updatestoc_newfile(QFileInfo info,int num)
{
    QByteArray sendmsg="";//发送的完整消息
    QByteArray recvmsg="";//收到的完整消息
    QByteArray sendcontent="";//发送的正文
    QByteArray recvcontent="";//收到的正文
    int sendlen;
    int recvlen;

    if (tcpsocket->isWritable())//如果允许发消息
    {
        QString relativepath=info.path();
        sendcontent+="request:newfile,relativepathlen:";
        sendcontent+=QString::number(relativepath.length(),10);
        sendcontent+=",relativepath:";
        sendcontent+=relativepath;
        sendcontent+=",num:";
        sendcontent+=QString::number(num,10);

        sendlen=sendcontent.length();
        sendmsg+=InttoQByteArray(sendlen);
        sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
        sendmsg+=sendcontent;
        tcpsocket->write(sendmsg);
        qDebug()<<"send: "<<sendcontent;
    }
    else
        return -10;
    if (tcpsocket->waitForReadyRead(5000))//如果5s内有消息收到
    {
        //验证消息的正确性
        recvmsg=tcpsocket->readAll();while (tcpsocket->waitForReadyRead(200)) recvmsg+=tcpsocket->readAll();qDebug()<<"recv bytes "<<recvmsg.length();
        if (recvmsg.length()<36)//收到消息不满36字节
            return -12;
        recvlen=QByteArraytoInt(recvmsg.mid(0,4));
        if (recvmsg.length()<36+recvlen)//收到消息长度不对
            return -12;
        recvcontent=recvmsg.mid(36,recvlen);
        if (QCryptographicHash::hash(recvcontent,QCryptographicHash::Md5).toHex()!=recvmsg.mid(4,32))//收到消息md5不对
            return -12;
        qDebug()<<"recv: "<<recvcontent;
        //确认消息含义
        if (recvcontent.mid(0,strlen("request:pardon"))=="request:pardon")//如果服务器要求重发
            return 0;
        if (recvcontent.mid(0,strlen("reponse:nonewfile"))=="reponse:nonewfile")//回应没有新文件了
        {
            return -1;
        }
        if (recvcontent.mid(0,strlen("response:newfile"))=="response:newfile")//如果服务器正常回应
        {
            // bool ok;
            int filenamelen=toint(recvcontent.mid(recvcontent.indexOf(",filenamelen:")+int(strlen(",filenamelen:")),10));
            QString filename=recvcontent.mid(recvcontent.indexOf(",filename:")+int(strlen(",filename:")),filenamelen);
            int pathlen=toint(recvcontent.mid(recvcontent.indexOf(",relativepathlen:")+int(strlen(",relativepathlen:")),10));
            QString path=recvcontent.mid(recvcontent.indexOf(",relativepath:")+int(strlen(",relativepath:")),pathlen);
            dir.cd(".");
            dir.cd(path);
            QString filepath=dir.filePath(filename);
            QFile* newfile=new QFile(filepath);
            QByteArray md5=recvcontent.mid(recvcontent.indexOf(",file:")+int(strlen(",file:")),32);
            int total=toint(recvcontent.mid(recvcontent.indexOf(",total:")+int(strlen(",total:")),10));
            on_recvfile(newfile,md5,total);
            return 1;

        }
        if (recvcontent=="response:pleaselogin")//如果服务器回应请登录
        {relogin();return 2;}
        return 10;
    }
    else
        return -11;
}

int MainWindow::tcp_updatectos()
{
    tcp_updatectos_head();
    QFileInfoList filelist,dirlist;
    dir.setPath(".");
    dirlist.append(QFileInfo("."));
    addlist(dir,dirlist,filelist);
    tcp_updatectos_all(dirlist,filelist);
    return 0;
}

int MainWindow::tcp_updatectos_head()
{
    QByteArray sendmsg="";//发送的完整消息
    QByteArray recvmsg="";//收到的完整消息
    QByteArray sendcontent="";//发送的正文
    QByteArray recvcontent="";//收到的正文
    int sendlen;
    int recvlen;

    if (tcpsocket->isWritable())//如果允许发消息
    {
        sendcontent+="request:updatectos";
        sendlen=sendcontent.length();
        sendmsg+=InttoQByteArray(sendlen);
        sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
        sendmsg+=sendcontent;
        tcpsocket->write(sendmsg);
        qDebug()<<"send: "<<sendcontent;
    }
    else
        return -10;
    if (tcpsocket->waitForReadyRead(5000))//如果5s内有消息收到
    {
        //验证消息的正确性
        recvmsg=tcpsocket->readAll();while (tcpsocket->waitForReadyRead(200)) recvmsg+=tcpsocket->readAll();qDebug()<<"recv bytes "<<recvmsg.length();
        if (recvmsg.length()<36)//收到消息不满36字节
            return -12;
        recvlen=QByteArraytoInt(recvmsg.mid(0,4));
        if (recvmsg.length()<36+recvlen)//收到消息长度不对
            return -12;
        recvcontent=recvmsg.mid(36,recvlen);
        if (QCryptographicHash::hash(recvcontent,QCryptographicHash::Md5).toHex()!=recvmsg.mid(4,32))//收到消息md5不对
            return -12;
        qDebug()<<"recv: "<<recvcontent;
        //确认消息含义
        if (recvcontent.mid(0,strlen("request:pardon"))=="request:pardon")//如果服务器要求重发
            return 0;
        if (recvcontent.mid(0,strlen("response:initialctos"))=="response:initialctos")//如果服务器正常回应
        {
            // bool ok;
            int code=toint(recvcontent.mid(int(strlen("response:initialctos,code:")),10));
            //if (ok)
            return code;
            //else return 10;
        }
        if (recvcontent=="response:pleaselogin")//如果服务器回应请登录
        {relogin();return 2;}
        return 10;
    }
    else
        return -11;
}

int MainWindow::tcp_updatectos_all(QFileInfoList&dirlist,QFileInfoList&filelist)
{
    QByteArray sendmsg="";//发送的完整消息
    QByteArray recvmsg="";//收到的完整消息
    QByteArray sendcontent="";//发送的正文
    QByteArray recvcontent="";//收到的正文
    int sendlen;
    int recvlen;
    QFile *file=new QFile();
    QByteArray md5,allcontent;
    int len,startfrom;
    if (tcpsocket->isWritable())//如果允许发消息
    {
        sendcontent+="request:senddirectory";

        sendlen=sendcontent.length();
        sendmsg+=InttoQByteArray(sendlen);
        sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
        sendmsg+=sendcontent;
        tcpsocket->write(sendmsg);
        qDebug()<<"send: "<<sendcontent;
    }
    else
        return -10;
receive:
    if (tcpsocket->waitForReadyRead(5000))//如果5s内有消息收到
    {
        //验证消息的正确性
        recvmsg=tcpsocket->readAll();while (tcpsocket->waitForReadyRead(200)) recvmsg+=tcpsocket->readAll();
        //qDebug()<<"recv bytes "<<recvmsg.length();
        if (recvmsg.length()<36)//收到消息不满36字节
            return -12;
        recvlen=QByteArraytoInt(recvmsg.mid(0,4));
        if (recvmsg.length()<36+recvlen)//收到消息长度不对
            return -12;
        recvcontent=recvmsg.mid(36,recvlen);
        if (QCryptographicHash::hash(recvcontent,QCryptographicHash::Md5).toHex()!=recvmsg.mid(4,32))//收到消息md5不对
            return -12;
        //qDebug()<<"recv "<<recvcontent;
        //确认消息含义
        if (recvcontent.mid(0,strlen("request:pardon"))=="request:pardon")//如果服务器要求重发
            return 0;
        else if (recvcontent.mid(0,strlen("request:directory1"))=="request:directory1")//如果服务器请求目录
        {
            //bool ok;
            int num=toint(recvcontent.mid(recvcontent.indexOf(",num:")+int(strlen(",num:")),10));
            // if (!ok)
            //   return 10;
            if (num>=dirlist.size()||num<0)//如果目录发完了
            {
                QByteArray sendmsg="";//发送的完整消息
                QByteArray sendcontent="";//发送的正文
                if (tcpsocket->isWritable())//如果允许发消息
                {
                    sendcontent+="response:directoryover1,num:";
                    sendcontent+=QString::number(num,10);

                    sendlen=sendcontent.length();
                    sendmsg+=InttoQByteArray(sendlen);
                    sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
                    sendmsg+=sendcontent;
                    tcpsocket->write(sendmsg);
                    qDebug()<<"send: "<<sendcontent;
                    goto receive;
                }
                else
                    return -10;
            }
            else
            {
                QByteArray sendmsg="";//发送的完整消息
                QByteArray sendcontent="";//发送的正文
                if (tcpsocket->isWritable())//如果允许发消息
                {
                    QFileInfo dirinfo=dirlist.at(num);
                    QString relativepath=dirinfo.filePath();
                    sendcontent+="response:directory1,num:";
                    sendcontent+=QString::number(num,10);
                    sendcontent+=",relativepathlen:";
                    sendcontent+=QString::number(relativepath.length(),10);
                    sendcontent+=",relativepath:";
                    sendcontent+=relativepath;

                    sendlen=sendcontent.length();
                    sendmsg+=InttoQByteArray(sendlen);
                    sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
                    sendmsg+=sendcontent;
                    tcpsocket->write(sendmsg);
                    qDebug()<<"send: "<<sendcontent;
                    goto receive;
                }
                else
                    return -10;
            }

        }
        else if (recvcontent.mid(0,strlen("request:file1"))=="request:file1")//如果服务器请求文件
        {
            //bool ok;
            int num=toint(recvcontent.mid(recvcontent.indexOf(",num:")+int(strlen(",num:")),10));
            // if (!ok)
            //     return 10;
            if (num>=filelist.size()||num<0)//如果文件发完了
            {
                QByteArray sendmsg="";//发送的完整消息
                QByteArray sendcontent="";//发送的正文
                if (tcpsocket->isWritable())//如果允许发消息
                {
                    sendcontent+="response:fileover1,num:";
                    sendcontent+=QString::number(num,10);

                    sendlen=sendcontent.length();
                    sendmsg+=InttoQByteArray(sendlen);
                    sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
                    sendmsg+=sendcontent;
                    tcpsocket->write(sendmsg);
                    qDebug()<<"send: "<<sendcontent;
                    goto receive;
                }
                else
                    return -10;
            }
            else
            {
                QByteArray sendmsg="";//发送的完整消息
                QByteArray sendcontent="";//发送的正文
                if (tcpsocket->isWritable())//如果允许发消息
                {
                    QFileInfo fileinfo=filelist.at(num);
                    QString relativepath=fileinfo.path();
                    QString filename=fileinfo.fileName();
                    file=new QFile(fileinfo.filePath());
                    file->open(QIODevice::ReadOnly);
                    allcontent=file->readAll();
                    md5=QCryptographicHash::hash(allcontent,QCryptographicHash::Md5).toHex();
                    file->close();
                    sendcontent+="response:file1,num:";
                    sendcontent+=QString::number(num,10);
                    sendcontent+=",relativepathlen:";
                    sendcontent+=QString::number(relativepath.length(),10);
                    sendcontent+=",relativepath:";
                    sendcontent+=relativepath;

                    sendcontent+=",filenamelen:";
                    sendcontent+=QString::number(filename.length(),10);
                    sendcontent+=",filename:";
                    sendcontent+=filename;

                    sendcontent+=",file:";
                    sendcontent+=md5;

                    sendlen=sendcontent.length();
                    sendmsg+=InttoQByteArray(sendlen);
                    sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
                    sendmsg+=sendcontent;
                    tcpsocket->write(sendmsg);
                    qDebug()<<"send: "<<sendcontent;
                    goto receive;
                }
                else
                    return -10;
            }

        }
        else if (recvcontent.mid(0,strlen("request:skipfile1"))=="request:skipfile1")//如果服务器回应跳过
        {
            //bool ok;
            int code=toint(recvcontent.mid(recvcontent.indexOf(",code:")+int(strlen(",code:")),10));
            //if (!ok)
            //    return 10;
            int num=toint(recvcontent.mid(recvcontent.indexOf(",num:")+int(strlen(",num:")),10));
            // if (!ok)
            //    return 10;
            if (code==1)//如果服务器秒传
            {
                QString filepath=filelist.at(num).filePath();
                if (userlog&&userlog->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
                {
                    QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
                    QString timestr = time.toString("yyyy-MM-dd HH:mm:ss: "); //设置显示格式
                    timestr+="file: ";
                    timestr+=filepath;
                    timestr+=" send in a second\n";
                    userlog->write(timestr.toLatin1());
                    userlog->close();
                }
            }
            QByteArray sendmsg="";//发送的完整消息
            QByteArray sendcontent="";//发送的正文
            if (tcpsocket->isWritable())//如果允许发消息
            {
                sendcontent+="response:nextfile1,num:";
                sendcontent+=QString::number(num,10);

                sendlen=sendcontent.length();
                sendmsg+=InttoQByteArray(sendlen);
                sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
                sendmsg+=sendcontent;
                tcpsocket->write(sendmsg);
                qDebug()<<"send: "<<sendcontent;
                goto receive;
            }
            else
                return -10;
        }
        else if (recvcontent.mid(0,strlen("request:recvfile"))=="request:recvfile")//如果服务器请求接受文件
        {
            //bool ok;
            //response:sendfile,file:{MD5},startfrom:{X},len:{X},content:{filestream}
            int pos1=recvcontent.indexOf(",relativepathlen:");
            int pos2=recvcontent.indexOf(",relativepath:");
            int pos3=recvcontent.indexOf(",filenamelen:");
            int pos4=recvcontent.indexOf(",filename:");
            int pos5=recvcontent.indexOf(",file:");
            int pos6=recvcontent.indexOf(",startfrom:");
            if (pos1<0||pos2<0||pos3<0||pos4<0||pos5<0||pos6<0)//没有这6段字符就判断为无法解析的回应
                return 10;
            int relativepathlen=toint(recvcontent.mid(pos1+int(strlen(",relativepathlen:")),10));
            //qDebug()<<recvcontent.mid(pos1+int(strlen(",relativepathlen:")),10);
            //if (!ok)//无法转换成数字，判断是无法解析的回应
            //    return 10;
            int filenamelen=toint(recvcontent.mid(pos3+int(strlen(",filenamelen:")),10));
            //if (!ok)//无法转换成数字，判断是无法解析的回应
            //    return 10;
            startfrom=toint(recvcontent.mid(pos6+int(strlen(",startfrom:")),10));
            //if (!ok)//无法转换成数字，判断是无法解析的回应
            //    return 10;
            QString relativepath=recvcontent.mid(pos2+int(strlen(",relativepath:")),relativepathlen);
            QString filename=recvcontent.mid(pos4+int(strlen(",filename:")),filenamelen);
            QByteArray newmd5=recvcontent.mid(pos5+int(strlen(",file:")),32);
            if (newmd5!=md5)
            {
                md5=newmd5;
                dir.setPath(".");
                dir.cd(relativepath);
                QString filepath=dir.filePath(filename);
                file=new QFile(filepath);
                file->open(QIODevice::ReadOnly);
                allcontent=file->readAll();
                file->close();
            }
            QFileInfo info;
            info.setFile(*file);
            transportdialog->reset_upfile(info.absoluteFilePath(),file->fileName(),int(info.size()));

            if (tcpsocket->isWritable())//如果允许发消息
            {
                len=allcontent.length()-startfrom;
                if (len<0)
                    len=0;
                else if (len>BLOCK_SIZE)
                    len=BLOCK_SIZE;
                sendmsg="";
                sendcontent="";
                sendcontent+="response:sendfile,relativepathlen:";
                sendcontent+=QString::number(relativepathlen,10);
                sendcontent+=",relativepath:";
                sendcontent+=relativepath;
                sendcontent+=",filenamelen:";
                sendcontent+=QString::number(filenamelen,10);
                sendcontent+=",filename:";
                sendcontent+=filename;
                sendcontent+=",file:";
                sendcontent+=md5;

                sendcontent+=",len:";
                sendcontent+=QString::number(len,10);
                sendcontent+=",content:";
                sendcontent+=allcontent.mid(startfrom,len);
                sendlen=sendcontent.length();
                sendmsg+=InttoQByteArray(sendlen);
                sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
                sendmsg+=sendcontent;
                tcpsocket->write(sendmsg);
                transportdialog->reset_upnow(startfrom+len);
                if (len==0)
                    transportdialog->reset_upfile("","",0);
                //qDebug()<<"send: "<<sendcontent;
                goto receive;
            }
            else
                return -10;

        }
        else if (recvcontent=="response:pleaselogin")//如果服务器回应请登录
        {relogin();return 2;}
        return 10;

    }
    return -11;
}

void MainWindow::update()
{
    timer->stop();
    if (logined&&path!="")//登陆并且绑定了
    {
        tcp_updatestoc();
        tcp_updatectos();
        timer->start(10000);
    }

}

int MainWindow::tcp_rebind()
{
    QByteArray sendmsg="";//发送的完整消息
    QByteArray recvmsg="";//收到的完整消息
    QByteArray sendcontent="";//发送的正文
    QByteArray recvcontent="";//收到的正文
    int sendlen;
    int recvlen;

    if (tcpsocket->isWritable())//如果允许发消息
    {
        sendcontent+="request:rebinddir,pathlen:";
        sendcontent+=QString::number(path.length());
        sendcontent+=",path:";
        sendcontent+=path;
        sendlen=sendcontent.length();

        sendmsg+=InttoQByteArray(sendlen);
        sendmsg+=QCryptographicHash::hash(sendcontent,QCryptographicHash::Md5).toHex();
        sendmsg+=sendcontent;
        tcpsocket->write(sendmsg);
        qDebug()<<"send: "<<sendcontent;
    }
    else
        return -10;
    if (tcpsocket->waitForReadyRead(5000))//如果5s内有消息收到
    {
        //验证消息的正确性
        recvmsg=tcpsocket->readAll();while (tcpsocket->waitForReadyRead(200)) recvmsg+=tcpsocket->readAll();qDebug()<<"recv bytes "<<recvmsg.length();
        if (recvmsg.length()<36)//收到消息不满36字节
            return -12;
        recvlen=QByteArraytoInt(recvmsg.mid(0,4));
        if (recvmsg.length()<36+recvlen)//收到消息长度不对
            return -12;
        recvcontent=recvmsg.mid(36,recvlen);
        if (QCryptographicHash::hash(recvcontent,QCryptographicHash::Md5).toHex()!=recvmsg.mid(4,32))//收到消息md5不对
            return -12;
        qDebug()<<"recv: "<<recvcontent;
        //确认消息含义
        if (recvcontent.mid(0,strlen("request:pardon"))=="request:pardon")//如果服务器要求重发
            return 0;
        if (recvcontent.mid(0,strlen("response:rebinddir"))=="response:rebinddir")//如果服务器正常回应
        {
            if (recvcontent.mid(recvcontent.indexOf(":rebinddir"))==sendcontent.mid(recvcontent.indexOf(":rebinddir")))
                return 1;
            else return 0;
        }
        else return 10;
    }
    else
        return -11;
}

void MainWindow::serverconn()
{
    serverconnecting=true;
    ConnectAction->setEnabled(false);

    if (mainlog->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
        QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
        QString timestr = time.toString("yyyy-MM-dd HH:mm:ss: "); //设置显示格式
        timestr+="Connect to server\n";
        mainlog->write(timestr.toLatin1());
    }
    mainlog->close();
    if (!logined)
    {
        LogoutAction->setEnabled(false);
        RebindAction->setEnabled(false);
        ConnectAction->setEnabled(false);
        UnbindActoin->setEnabled(false);
        LoginAction->setEnabled(true);
    }
    else
    {
        LogoutAction->setEnabled(true);
        RebindAction->setEnabled(true);
        ConnectAction->setEnabled(true);
        UnbindActoin->setEnabled(true);
        LoginAction->setEnabled(false);
    }
}

void MainWindow::serverdisconn()
{
    serverconnecting=false;
    ConnectAction->setEnabled(true);

    if (mainlog->open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)){
        QDateTime time = QDateTime::currentDateTime();//获取系统现在的时间
        QString timestr = time.toString("yyyy-MM-dd HH:mm:ss: "); //设置显示格式
        timestr+="Disconnect from server\n";
        mainlog->write(timestr.toLatin1());
    }
    mainlog->close();
    LogoutAction->setEnabled(false);
    RebindAction->setEnabled(false);
    ConnectAction->setEnabled(false);
    UnbindActoin->setEnabled(false);
    LoginAction->setEnabled(false);
}

int  MainWindow::QByteArraytoInt(QByteArray b)
{
    char intbytes[4];
    int  i;
    intbytes[0]=b[0];
    intbytes[1]=b[1];
    intbytes[2]=b[2];
    intbytes[3]=b[3];
    memcpy(&i,intbytes,4);
    return i;
}

QByteArray MainWindow::InttoQByteArray(int i)
{
    char intbytes[4];
    memcpy (intbytes,&i,4);
    return QByteArray(intbytes,4);
}

void MainWindow::addlist(QDir dir,QFileInfoList&dirlist,QFileInfoList&filelist)
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
            dirlist+=fileInfo.filePath();
            qDebug()<<"XXXXXXX"<<fileInfo.filePath();
            addlist(QDir(fileInfo.filePath()),dirlist,filelist);
        }
        else
        {
            if (fileInfo.size()!=0&&fileInfo.fileName().mid(0,2)!="~$"&&fileInfo.suffix()!="sjtempfile"&&fileInfo.size()<MAX_FILE_SIZE)
                filelist+=fileInfo;
        }
        i++;
    }
    while(i<list.size());
}
