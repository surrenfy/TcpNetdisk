#include <transportdialog.h>

TransportDialog::TransportDialog(QIcon icon)
{
    setWindowIcon(icon);
    setWindowTitle(tr("Transport table"));
    QFont font;
    font.setFamily("Comic Sans MS");
    font.setPointSize(10);
    setFont(font);
    ufilenameLabel=new QLabel();
    dfilenameLabel=new QLabel();
    usizeLabel=new QLabel();
    dsizeLabel=new QLabel();
    uspeedLabel=new QLabel("0B/s");
    dspeedLabel=new QLabel("0B/s");
    utimeLabel=new QLabel("--:--:--");
    dtimeLabel=new QLabel("--:--:--");
    upProgressBar=new QProgressBar();
    downProgressBar=new QProgressBar();
    upProgressBar->setFixedHeight(14);
    downProgressBar->setFixedHeight(14);
    upProgressBar->setValue(0);
    downProgressBar->setValue(0);
    QLabel * label1=new QLabel("upload:");
    QLabel * label2=new QLabel("download:");
    mainLayout=new QGridLayout();
    mainLayout->addWidget(label2,1,0);
    mainLayout->addWidget(dfilenameLabel,1,1);
    mainLayout->addWidget(dsizeLabel,1,2);
    mainLayout->addWidget(downProgressBar,1,3,1,2);
    mainLayout->addWidget(dspeedLabel,1,5);
    mainLayout->addWidget(dtimeLabel,1,6);

    mainLayout->addWidget(label1,0,0);
    mainLayout->addWidget(ufilenameLabel,0,1);
    mainLayout->addWidget(usizeLabel,0,2);
    mainLayout->addWidget(upProgressBar,0,3,1,2);
    mainLayout->addWidget(uspeedLabel,0,5);
    mainLayout->addWidget(utimeLabel,0,6);
    upsize=0;
    downsize=0;
    upnow=0;
    downnow=0;
    ulast=0;
    dlast=0;
    uspeed=0;
    dspeed=0;
    zero.setHMS(0,0,0,0);
    setLayout(mainLayout);
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);

}

void TransportDialog::reset_upfile(QString absolutepath,QString filename,int size)
{
    ufilenameLabel->setText(filename);
    ufilenameLabel->setToolTip(absolutepath);
    upsize=size;
    ulast=0;
    uspeed=0;
    upnow=0;
}

void TransportDialog::reset_downfile(QString absolutepath,QString filename,int size)
{
    dfilenameLabel->setText(filename);
    dfilenameLabel->setToolTip(absolutepath);
    downsize=size;
    dlast=0;
    dspeed=0;
    downnow=0;
}

void TransportDialog::reset_upnow(int size)
{
    upnow=size;
}

void TransportDialog::reset_downnow(int size)
{
    downnow=size;
}

void TransportDialog::update()
{
    if (ufilenameLabel->text()!="")
    {
        uspeed=upnow-ulast;
        ulast=upnow;
        if (uspeed<=0)//0
        {
            uspeedLabel->setText("0B/s");
            utimeLabel->setText("--:--:--");
        }
        else if(uspeed<1024)//B
        {
            QString temp=QString::number(uspeed,10);
            temp+="B/s";
            uspeedLabel->setText(temp);

            double x=upnow/upsize;
            x*=100;
            upProgressBar->setValue(int(x));
            int time=(upsize-upnow)/uspeed;

            if (time>=86400)
                utimeLabel->setText("--:--:--");
            else
            {
                QTime t=zero.addSecs(time);
                utimeLabel->setText(t.toString("HH:mm:ss"));
            }
        }
        else if (uspeed<1024*1024)//K
        {
            QString temp=QString::number(uspeed/1024,10);
            temp+="KB/s";
            uspeedLabel->setText(temp);

            double x=upnow/upsize;
            x*=100;
            upProgressBar->setValue(int(x));
            int time=(upsize-upnow)/uspeed;

            if (time>=86400)
                utimeLabel->setText("--:--:--");
            else
            {
                QTime t=zero.addSecs(time);
                utimeLabel->setText(t.toString("HH:mm:ss"));
            }
        }
        else if (uspeed<1024*1024)//M
        {
            QString temp=QString::number(uspeed/(1024*1024),10);
            temp+="MB/s";
            uspeedLabel->setText(temp);

            double x=upnow/upsize;
            x*=100;
            upProgressBar->setValue(int(x));
            int time=(upsize-upnow)/uspeed;

            if (time>=86400)
                utimeLabel->setText("--:--:--");
            else
            {
                QTime t=zero.addSecs(time);
                utimeLabel->setText(t.toString("HH:mm:ss"));
            }
        }
        else if(uspeed<1024*1024)//G
        {
            QString temp=QString::number(uspeed/(1024*1024*1024),10);
            temp+="GB/s";
            uspeedLabel->setText(temp);

            double x=upnow/upsize;
            x*=100;
            upProgressBar->setValue(int(x));
            int time=(upsize-upnow)/uspeed;

            if (time>=86400)
                utimeLabel->setText("--:--:--");
            else
            {
                QTime t=zero.addSecs(time);
                utimeLabel->setText(t.toString("HH:mm:ss"));
            }
        }
    }
    else
    {
        uspeedLabel->setText("0B/s");
        upProgressBar->setValue(0);
        utimeLabel->setText("--:--:--");

    }
    if (dfilenameLabel->text()!="")
    {
        dspeed=downnow-dlast;
        dlast=downnow;
        if (dspeed<=0)//0
        {
            dspeedLabel->setText("0B/s");
            dtimeLabel->setText("--:--:--");
        }
        else if(dspeed<1024)//B
        {
            QString temp=QString::number(dspeed,10);
            temp+="B/s";
            dspeedLabel->setText(temp);

            double x=downnow/downsize;
            x*=100;
            downProgressBar->setValue(int(x));
            int time=(downsize-downnow)/dspeed;

            if (time>=86400)
                dtimeLabel->setText("--:--:--");
            else
            {
                QTime t=zero.addSecs(time);
                dtimeLabel->setText(t.toString("HH:mm:ss"));
            }
        }
        else if (dspeed<1024*1024)//K
        {
            QString temp=QString::number(dspeed/1024,10);
            temp+="KB/s";
            dspeedLabel->setText(temp);

            double x=downnow/downsize;
            x*=100;
            downProgressBar->setValue(int(x));
            int time=(downsize-downnow)/dspeed;

            if (time>=86400)
                dtimeLabel->setText("--:--:--");
            else
            {
                QTime t=zero.addSecs(time);
                dtimeLabel->setText(t.toString("HH:mm:ss"));
            }
        }
        else if (dspeed<1024*1024)//M
        {
            QString temp=QString::number(dspeed/(1024*1024),10);
            temp+="MB/s";
            dspeedLabel->setText(temp);

            double x=downnow/downsize;
            x*=100;
            downProgressBar->setValue(int(x));
            int time=(downsize-downnow)/dspeed;

            if (time>=86400)
                dtimeLabel->setText("--:--:--");
            else
            {
                QTime t=zero.addSecs(time);
                dtimeLabel->setText(t.toString("HH:mm:ss"));
            }
        }
        else if(dspeed<1024*1024)//G
        {
            QString temp=QString::number(dspeed/(1024*1024*1024),10);
            temp+="GB/s";
            dspeedLabel->setText(temp);

            double x=downnow/downsize;
            x*=100;
            downProgressBar->setValue(int(x));
            int time=(downsize-downnow)/dspeed;

            if (time>=86400)
                dtimeLabel->setText("--:--:--");
            else
            {
                QTime t=zero.addSecs(time);
                dtimeLabel->setText(t.toString("HH:mm:ss"));
            }
        }
    }
    else
    {
        dspeedLabel->setText("0B/s");
        downProgressBar->setValue(0);
        dtimeLabel->setText("--:--:--");
    }
    repaint();
}
