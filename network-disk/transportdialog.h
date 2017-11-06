#ifndef TRANSPORTDIALOG_H
#define TRANSPORTDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QGridLayout>
#include <QIcon>
#include <QRegExpValidator>
#include <QProgressBar>
#include <QTime>
#include <QTimer>

class TransportDialog:public QDialog
{
    Q_OBJECT
public:
    TransportDialog(QIcon);
    friend class MainWindow;
private:
    QGridLayout* mainLayout;
    QLabel* ufilenameLabel;
    QLabel* dfilenameLabel;
    QLabel* usizeLabel;
    QLabel* dsizeLabel;
    QLabel* uspeedLabel;
    QLabel* dspeedLabel;
    QLabel* utimeLabel;
    QLabel* dtimeLabel;
    QProgressBar* upProgressBar;
    QProgressBar* downProgressBar;
    int          upsize;
    int          downsize;
    int          upnow;
    int          downnow;
	int          ulast;
	int          dlast;
    int          uspeed;
    int          dspeed;
    QTime        zero;
    QDateTime    lastupdateDateTime;

private:
    void reset_upfile(QString absolutepath,QString filename,int size);
    void reset_downfile(QString absolutepath,QString filename,int size);
    void reset_upnow(int);
    void reset_downnow(int);
private slots:
    void update();

};

#endif // TRANSPORTDIALOG_H
