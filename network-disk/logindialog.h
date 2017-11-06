#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QTabWidget>
#include <QIcon>
#include <QRegExpValidator>

class LoginDialog:public QDialog
{
    Q_OBJECT
public:
    LoginDialog(QIcon);
    friend class MainWindow;
private:
    QHBoxLayout* mainLayout;
    QTabWidget* tab;
    QWidget* loginWidget;
    QWidget* registerWidget;

    QLabel* login_nameLabel;
    QLineEdit* login_nameEdit;
    QLabel* login_passwordLabel;
    QLineEdit* login_passwordEdit;
    QCheckBox* login_remember;
    QPushButton* login_okButton;
    QPushButton* login_cancelButton;
    QDialogButtonBox* login_Buttonbox;
    QFormLayout* login_Form;
    QVBoxLayout* login_Layout;
    QLabel* login_suggest;
    QLabel* register_suggest;

    QLabel* register_nameLabel;
    QLineEdit* register_nameEdit;
    QLabel* register_passwordLabel;
    QLineEdit* register_passwordEdit;
    QCheckBox* register_remember;
    QLabel* register_passwordconfirmLabel;
    QLineEdit* register_passwordconfirmEdit;
    QPushButton* register_okButton;
    QPushButton* register_cancelButton;
    QDialogButtonBox* register_Buttonbox;
    QFormLayout* register_Form;
    QVBoxLayout* register_Layout;
    bool checkpassword(QString);
    bool checkusername(QString);
private slots:
    void on_register_passwordconfirmEdit_changed();
    void on_register_passwordEdit_changed();
    void on_register_nameEdit_changed();
    void on_login_nameEdit_changed();
    void on_login_passwordEdit_changed();
};

#endif // LOGINDIALOG_H
