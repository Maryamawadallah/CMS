#ifndef MENUDOC_H
#define MENUDOC_H

#include <QMainWindow>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QDateEdit>
#include <QInputDialog>

namespace Ui {
class MenuDoc;
}

class MenuDoc : public QMainWindow
{
    Q_OBJECT

public:
    explicit MenuDoc(QWidget *parent = nullptr);
    ~MenuDoc();

private slots:
    void on_pushButton_view_clicked();  // Button for viewing appointments
    void on_pushButton_Report_clicked(); // Button for generating patient report
    void on_pushButton_Exit_clicked();   // Button for exiting

private:
    Ui::MenuDoc *ui;
};

#endif // MENUDOC_H
