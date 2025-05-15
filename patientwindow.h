#ifndef PATIENTWINDOW_H
#define PATIENTWINDOW_H

#include <QMainWindow>
#include <vector>
#include "patient.h"
#include "appointment.h"

namespace Ui {
class PatientWindow;
}

class PatientWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PatientWindow(QWidget *parent = nullptr);
    ~PatientWindow();

    // دالة للتحقق من وجود المريض بناءً على الـ ID
    bool isPatientExisting(const QString& id);

signals:
    // إشارة جديدة لتمرير بيانات الموعد إلى نافذة أخرى (زي MenuDoc مثلاً)
    void appointmentAdded(const QString &name, const QString &date, const QString &time);

private slots:
    void on_doctorLoginButton_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();

private:
    Ui::PatientWindow *ui;
    std::vector<Patient> patients;
    std::vector<Appointment> appointments;
    int lastPatientId = 0;

    void loadData();
    void saveData();
    bool isOnlyLetters(const std::string& str);

    // دوال التحقق من صحة المدخلات
    bool isValidDate(const QString& date);
    bool isValidTime(const QString& time);
};

#endif // PATIENTWINDOW_H
