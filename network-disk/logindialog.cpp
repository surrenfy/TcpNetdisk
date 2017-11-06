#include "logindialog.h"

LoginDialog::LoginDialog(QIcon icon)
{
    setWindowTitle(tr("Login/Register"));
    login_suggest=new QLabel(tr(""));
    register_suggest=new QLabel(tr(""));
    login_nameLabel=new QLabel(tr("Username"));
    login_nameEdit = new QLineEdit;
    login_passwordLabel=new QLabel(tr("Password"));
    login_passwordEdit = new QLineEdit;
    login_passwordEdit->setEchoMode(QLineEdit::Password);
    login_remember=new QCheckBox(tr("Remember me"));

    login_okButton=new QPushButton(tr("Login"));
    login_cancelButton=new QPushButton(tr("Cancel"));
    login_Buttonbox=new QDialogButtonBox(Qt::Horizontal);
    login_Buttonbox->addButton(login_okButton,QDialogButtonBox::ActionRole);
    login_Buttonbox->addButton(login_cancelButton,QDialogButtonBox::ActionRole);

    login_Form=new QFormLayout;
    login_Form->addRow(login_nameLabel,login_nameEdit);
    login_Form->addRow(login_passwordLabel,login_passwordEdit);

    login_Layout = new QVBoxLayout;
    login_Layout->addLayout(login_Form);
    login_Layout->addWidget(login_remember);
    login_Layout->addWidget(login_suggest);
    login_Layout->addWidget(login_Buttonbox);

    loginWidget=new QWidget;
    loginWidget->setLayout(login_Layout);

    register_nameLabel=new QLabel(tr("Username"));
    register_nameEdit = new QLineEdit;
    register_passwordLabel=new QLabel(tr("Password"));
    register_passwordEdit = new QLineEdit;
    register_passwordconfirmLabel=new QLabel(tr("Confirm"));
    register_passwordconfirmEdit = new QLineEdit;
    register_passwordEdit->setEchoMode(QLineEdit::Password);
    register_passwordconfirmEdit->setEchoMode(QLineEdit::Password);
    register_okButton=new QPushButton(tr("Submit"));
    register_cancelButton=new QPushButton(tr("Cancel"));
    register_Buttonbox=new QDialogButtonBox(Qt::Horizontal);
    register_Buttonbox->addButton(register_okButton,QDialogButtonBox::ActionRole);
    register_Buttonbox->addButton(register_cancelButton,QDialogButtonBox::ActionRole);
    register_remember=new QCheckBox(tr("Remember me"));
    register_Form=new QFormLayout;
    register_Form->addRow(register_nameLabel,register_nameEdit);
    register_Form->addRow(register_passwordLabel,register_passwordEdit);
    register_Form->addRow(register_passwordconfirmLabel,register_passwordconfirmEdit);
    register_Layout = new QVBoxLayout;
    register_Layout->addLayout(register_Form);
    register_Layout->addWidget(register_remember);
    register_Layout->addWidget(register_suggest);
    register_Layout->addWidget(register_Buttonbox);

    registerWidget=new QWidget;
    registerWidget->setLayout(register_Layout);

    tab=new QTabWidget;
    tab->addTab(loginWidget,"Login");
    tab->addTab(registerWidget,"Register");
    mainLayout=new QHBoxLayout;
    mainLayout->addWidget(tab);
    setLayout(mainLayout);
    resize(360,222);
    QFont font;
    font.setFamily("Comic Sans MS");
    font.setPointSize(10);
    setFont(font);
    setWindowIcon(icon);
    register_okButton->setDisabled(true);
    login_okButton->setDisabled(true);
    login_nameEdit->setMaxLength(32);
    login_passwordEdit->setMaxLength(32);
    register_nameEdit->setMaxLength(32);
    register_passwordEdit->setMaxLength(32);
    register_passwordconfirmEdit->setMaxLength(32);

    QRegExp regx("[a-zA-Z0-9_]+$");
    QValidator *validator = new QRegExpValidator(regx, register_nameEdit);
    register_nameEdit->setValidator( validator );
    login_nameEdit->setValidator(validator);

    connect(login_cancelButton,SIGNAL(clicked()),this,SLOT(close()));
    connect(register_cancelButton,SIGNAL(clicked()),this,SLOT(close()));

    connect(login_passwordEdit,SIGNAL(textChanged(const QString&)),this,SLOT(on_login_passwordEdit_changed()));
    connect(login_nameEdit,SIGNAL(textChanged(const QString&)),this,SLOT(on_login_nameEdit_changed()));

    connect(register_passwordconfirmEdit,SIGNAL(textChanged(const QString&)),this,SLOT(on_register_passwordconfirmEdit_changed()));
    connect(register_passwordEdit,SIGNAL(textChanged(const QString&)),this,SLOT(on_register_passwordEdit_changed()));
    connect(register_nameEdit,SIGNAL(textChanged(const QString&)),this,SLOT(on_register_nameEdit_changed()));

}

void LoginDialog::on_login_nameEdit_changed()
{
    if (login_passwordEdit->text()!=""&&login_nameEdit->text()!="")
        login_okButton->setEnabled(true);
    else login_okButton->setEnabled(false);
}

void LoginDialog::on_login_passwordEdit_changed()
{
    if (login_passwordEdit->text()!=""&&login_nameEdit->text()!="")
        login_okButton->setEnabled(true);
    else login_okButton->setEnabled(false);
}

void LoginDialog::on_register_nameEdit_changed()
{
    if (checkpassword(register_passwordEdit->text())&&register_passwordEdit->text()==register_passwordconfirmEdit->text()&&register_nameEdit->text()!="")
    {
        register_suggest->setText("now you can sunmit");
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::darkGreen);
        register_suggest->setPalette(pa);
        register_okButton->setDisabled(false);
    }
    else if (register_nameEdit->text()=="")
    {
        register_okButton->setDisabled(true);
        register_suggest->setText("please input your username");
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::darkRed);
        register_suggest->setPalette(pa);
    }
    else if (register_passwordEdit->text()!=register_passwordconfirmEdit->text()&&register_passwordconfirmEdit->text()!="")
    {
        register_okButton->setDisabled(true);
        register_suggest->setText("twice input of password are different");
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::darkRed);
        register_suggest->setPalette(pa);
    }
}

void LoginDialog::on_register_passwordconfirmEdit_changed()
{
    if (checkpassword(register_passwordEdit->text())&&register_passwordEdit->text()==register_passwordconfirmEdit->text()&&register_nameEdit->text()!="")
    {
        register_suggest->setText("now you can sunmit");
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::darkGreen);
        register_suggest->setPalette(pa);
        register_okButton->setDisabled(false);
    }
    else if (register_nameEdit->text()=="")
    {
        register_okButton->setDisabled(true);
        register_suggest->setText("please input your username");
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::darkRed);
        register_suggest->setPalette(pa);
    }
    else if (register_passwordEdit->text()!=register_passwordconfirmEdit->text()&&register_passwordconfirmEdit->text()!="")
    {
        register_okButton->setDisabled(true);
        register_suggest->setText("twice input of password are different");
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::darkRed);
        register_suggest->setPalette(pa);
    }
}

void LoginDialog::on_register_passwordEdit_changed()
{
    if (checkpassword(register_passwordEdit->text())&&register_passwordEdit->text()==register_passwordconfirmEdit->text()&&register_nameEdit->text()!="")
    {
        register_suggest->setText("now you can sunmit");
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::darkGreen);
        register_suggest->setPalette(pa);
        register_okButton->setDisabled(false);
    }
    else if (register_nameEdit->text()=="")
    {
        register_okButton->setDisabled(true);
        register_suggest->setText("please input your username");
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::darkRed);
        register_suggest->setPalette(pa);
    }
    else if (register_passwordEdit->text()!=register_passwordconfirmEdit->text()&&register_passwordconfirmEdit->text()!="")
    {
        register_okButton->setDisabled(true);
        register_suggest->setText("twice input of password are different");
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::darkRed);
        register_suggest->setPalette(pa);
    }
}

bool LoginDialog::checkpassword(QString password)
{
    if (password.length()<6)
    {
        register_suggest->setText("your password should be longer than 5");
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::darkRed);
        register_suggest->setPalette(pa);
        return false;
    }
    if (password.length()>32)
    {
        register_suggest->setText("your password should be no longer than 32");
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::darkRed);
        register_suggest->setPalette(pa);
        return false;
    }
    int type[4]={0,0,0,0};
    int i;
    QString special="~!@#$%^&*()_+-=[]\\{}|:\'\";<>?,./";
    for (i=0;i<password.length();i++)
        if (password[i]>='A'&&password[i]<='Z')
            type[0]=1;
        else if(password[i]>='a'&&password[i]<='z')
            type[1]=1;
        else if(password[i]>='0'&&password[i]<='9')
            type[2]=1;
        else if(special.contains(password[i]))
            type[3]=1;
        else{
            register_suggest->setText("your password contains illegal character");
            QPalette pa;
            pa.setColor(QPalette::WindowText,Qt::darkRed);
            register_suggest->setPalette(pa);
            return false;
        }
    if ((type[0]+type[1]+type[2]+type[3])<3)
    {

        register_suggest->setText("your password should contains at least 3 types of character");
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::darkRed);
        register_suggest->setPalette(pa);
        return false;
    }

    if (password==register_nameEdit->text())
    {
        register_suggest->setText("your password is the same with username");
        QPalette pa;
        pa.setColor(QPalette::WindowText,Qt::darkRed);
        register_suggest->setPalette(pa);
        return false;
    }
    register_suggest->setText("good password");
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::darkGreen);
    register_suggest->setPalette(pa);
    return true;
}

