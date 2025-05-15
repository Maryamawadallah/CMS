#ifndef PATIENTWINDOW_H
#define PATIENTWINDOW_H

#include <QMainWindow>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>
#include <QMessageBox>
#include <QInputDialog>
#include <vector>
#include <string>

QT_BEGIN_NAMESPACE
namespace Ui { class PatientWindow; }
QT_END_NAMESPACE

class Patient {
public:
    Patient(const std::string& name = "", const std::string& id = "", const std::string& history = "");

    std::string getName() const;
    std::string getId() const;
    std::string getHistory() const;

    QJsonObject toJson() const;
    static Patient fromJson(const QJsonObject& json);

private:
    std::string name;
    std::string id;
    std::string history;
};

class Appointment {
public:
    Appointment(const std::string& patientId = "", const std::string& date = "", const std::string& time = "");

    std::string getPatientId() const;
    std::string getDate() const;
    std::string getTime() const;
    void setDate(const std::string& newDate);
    void setTime(const std::string& newTime);

    QJsonObject toJson() const;
    static Appointment fromJson(const QJsonObject& json);

private:
    std::string patientId;
    std::string date;
    std::string time;
};

class PatientWindow : public QMainWindow
{
    Q_OBJECT

public:
    PatientWindow(QWidget *parent = nullptr);
    ~PatientWindow();

private slots:
    void on_pushButton_clicked(); // Add Patient
    void on_pushButton_2_clicked(); // Book Appointment
    void on_pushButton_3_clicked(); // View Appointments
    void on_pushButton_4_clicked(); // Cancel Appointment
    void on_pushButton_5_clicked(); // Update Appointment

private:
    Ui::PatientWindow *ui;
    std::vector<Patient> patients;
    std::vector<Appointment> appointments;
    int lastPatientId = 0;

    bool isOnlyLetters(const std::string& str);
    bool isValidDate(const QString& date);
    bool isValidTime(const QString& time);
    bool patientExists(const QString& id);
    bool appointmentExists(const QString& date, const QString& time);

    void loadData();
    void saveData();
};

#endif // PATIENTWINDOW_H
