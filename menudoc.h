#ifndef MENUDOC_H
#define MENUDOC_H

#include <QMainWindow>
#include <QVector>
#include <QStringList>
#include <QRegularExpression>
#include <QMap>

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
    void on_pushButton_Report_clicked();
    void on_pushButton_view_clicked();
    void on_pushButton_Exit_clicked();

private:
    Ui::MenuDoc *ui;
    void createFileIfNotExists(const QString &filename, const QString &header);
    QVector<QStringList> readAppointmentsData();
    void writeToReport(const QString &content);
    bool validateDate(const QString &date);
    bool validateTime(const QString &time);
    void showError(const QString &message);
    void logInvalidEntry(const QString &line); // ✅ أضفناها لتسجيل الإدخالات الغلط
};

#endif // MENUDOC_H
