#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEdit_Password->setEchoMode(QLineEdit::Password);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString Password = ui->lineEdit_Password->text();
    if (Password == "doctor123") {
        QMessageBox::information(this, "Login Success", "Welcome, Doctor!");
        this->hide();
        MenuDoc *menudoc = new MenuDoc();
        menudoc->show();
    } else {
        QMessageBox::warning(this, "Invalid Password", "Please Enter a Valid Password");
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Exit", "Are you sure to close the application?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        QApplication::quit();
    }
}
