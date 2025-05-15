#include "patientwindow.h"
#include "ui_patientwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include <algorithm>
#include <QFile>
#include <QTextStream>
#include <regex>
#include <QDir>
#include <QRegularExpression>
#include "mainwindow.h"

void PatientWindow::on_doctorLoginButton_clicked()
{
    this->hide(); // إخفاء نافذة المرضى
    MainWindow *loginWindow = new MainWindow();
    loginWindow->show();
}


PatientWindow::PatientWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PatientWindow)
{
    ui->setupUi(this);
    loadData();
}

PatientWindow::~PatientWindow()
{
    delete ui;
    saveData();
}

// دالة للتأكد من أن الاسم يحتوي على حروف فقط
bool PatientWindow::isOnlyLetters(const std::string& str) {
    return std::regex_match(str, std::regex("^[A-Za-z ]+$"));
}

// دالة للتحقق من صحة التاريخ
bool PatientWindow::isValidDate(const QString& date) {
    QRegularExpression dateRegex("^([0-2][0-9]|(3)[0-1])/(0[1-9]|1[0-2])/[0-9]{4}$");
    return dateRegex.match(date).hasMatch();
}

// دالة للتحقق من صحة الوقت
bool PatientWindow::isValidTime(const QString& time) {
    QRegularExpression timeRegex("^[0-1][0-9]|2[0-3]:[0-5][0-9]$");
    return timeRegex.match(time).hasMatch();
}

// دالة للتأكد إذا كان المريض موجودًا بالفعل
bool PatientWindow::isPatientExisting(const QString& id) {
    for (const auto& p : patients) {
        if (QString::fromStdString(p.getId()) == id) {
            return true; // المريض موجود بالفعل
        }
    }
    return false; // المريض غير موجود
}

void PatientWindow::loadData() {
    QDir dir(QDir::currentPath() + "/Data");
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QFile patientFile(dir.filePath("patients.csv"));
    if (patientFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&patientFile);
        QString header = in.readLine();
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList fields = line.split(",");
            if (fields.size() >= 3) {
                QString name = fields[0];
                QString id = fields[1];
                QString history = fields[2];
                patients.emplace_back(name.toStdString(), id.toStdString(), history.toStdString());
                lastPatientId = std::max(lastPatientId, id.toInt());
            }
        }
        patientFile.close();
    }

    QFile appointmentFile(dir.filePath("appointments.csv"));
    if (appointmentFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&appointmentFile);
        QString header = in.readLine();
        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList fields = line.split(",");
            if (fields.size() >= 3) {
                QString patientId = fields[0];
                QString date = fields[1];
                QString time = fields[2];
                appointments.emplace_back(patientId.toStdString(), date.toStdString(), time.toStdString());
            }
        }
        appointmentFile.close();
    }
}

void PatientWindow::saveData() {
    QDir dir(QDir::currentPath() + "/Data");
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QFile patientFile(dir.filePath("patients.csv"));
    if (patientFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&patientFile);
        out << "Name,ID,History\n";
        for (const auto& p : patients) {
            out << QString::fromStdString(p.getName()) << ","
                << QString::fromStdString(p.getId()) << ","
                << QString::fromStdString(p.getHistory()) << "\n";
        }
        patientFile.close();
    }

    QFile appointmentFile(dir.filePath("appointments.csv"));
    if (appointmentFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&appointmentFile);
        out << "PatientID,Date,Time\n";
        for (const auto& a : appointments) {
            out << QString::fromStdString(a.getPatientId()) << ","
                << QString::fromStdString(a.getDate()) << ","
                << QString::fromStdString(a.getTime()) << "\n";
        }
        appointmentFile.close();
    }

    QFile mergedFile(dir.filePath("patients_appointments.csv"));
    if (mergedFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&mergedFile);
        out << "Name,ID,History,Date,Time\n";

        for (const auto& p : patients) {
            bool appointmentFound = false;

            for (const auto& a : appointments) {
                if (a.getPatientId() == p.getId()) {
                    if (!appointmentFound) {
                        out << QString::fromStdString(p.getName()) << ","
                            << QString::fromStdString(p.getId()) << ","
                            << QString::fromStdString(p.getHistory()) << ","
                            << QString::fromStdString(a.getDate()) << ","
                            << QString::fromStdString(a.getTime()) << "\n";
                        appointmentFound = true;
                    } else {
                        out << ",,,"  // خانات فاضية للمريض نفسه
                            << QString::fromStdString(a.getDate()) << ","
                            << QString::fromStdString(a.getTime()) << "\n";
                    }
                }
            }

            if (!appointmentFound) {
                out << QString::fromStdString(p.getName()) << ","
                    << QString::fromStdString(p.getId()) << ","
                    << QString::fromStdString(p.getHistory()) << ",,\n";
            }
        }

        mergedFile.close();
    }

    QMessageBox::information(this, "Data Saved", "All data saved successfully.");
}

void PatientWindow::on_pushButton_clicked()
{
    bool ok1, ok2;
    QString name = QInputDialog::getText(this, "Add Patient", "Enter Patient Name:", QLineEdit::Normal, "", &ok1);
    if (!ok1 || name.trimmed().isEmpty()) return;

    if (!isOnlyLetters(name.toStdString())) {
        QMessageBox::warning(this, "Invalid Name", "Name must contain only letters and spaces.");
        return;
    }

    QString history = QInputDialog::getText(this, "Add Patient", "Enter Medical History:", QLineEdit::Normal, "", &ok2);
    if (!ok2 || history.trimmed().isEmpty()) return;

    lastPatientId++;
    QString id = QString::number(lastPatientId);

    // التأكد من أن المريض غير موجود مسبقًا
    if (isPatientExisting(id)) {
        QMessageBox::warning(this, "Patient Exists", "Patient with this ID already exists.");
        return;
    }

    patients.emplace_back(name.toStdString(), id.toStdString(), history.toStdString());

    saveData();
    QMessageBox::information(this, "Success", "Patient added with ID: " + id);
}

void PatientWindow::on_pushButton_2_clicked()
{
    bool ok1, ok2, ok3;
    QString id = QInputDialog::getText(this, "Book Appointment", "Enter Patient ID:", QLineEdit::Normal, "", &ok1);
    QString date = QInputDialog::getText(this, "Book Appointment", "Enter Date (DD/MM/YYYY):", QLineEdit::Normal, "", &ok2);
    QString time = QInputDialog::getText(this, "Book Appointment", "Enter Time (HH:MM):", QLineEdit::Normal, "", &ok3);

    if (!ok1 || !ok2 || !ok3 || id.isEmpty() || date.isEmpty() || time.isEmpty()) return;

    if (!isValidDate(date)) {
        QMessageBox::warning(this, "Invalid Date", "Date must be in the format DD/MM/YYYY.");
        return;
    }

    if (!isValidTime(time)) {
        QMessageBox::warning(this, "Invalid Time", "Time must be in the format HH:MM.");
        return;
    }

    if (!isPatientExisting(id)) {
        QMessageBox::warning(this, "Error", "Patient ID not found.");
        return;
    }

    for (const auto& a : appointments) {
        if ((a.getDate() == date.toStdString()) && (a.getTime() == time.toStdString())) {
            QMessageBox::warning(this, "Error", "This time is already booked.");
            return;
        }
    }

    appointments.emplace_back(id.toStdString(), date.toStdString(), time.toStdString());
    saveData();
    QMessageBox::information(this, "Success", "Appointment booked.");
}

void PatientWindow::on_pushButton_3_clicked()
{
    bool ok;
    QString id = QInputDialog::getText(this, "View Appointments", "Enter Patient ID:", QLineEdit::Normal, "", &ok);
    if (!ok || id.isEmpty()) return;

    QString result = "Appointments for Patient ID " + id + ":\n";
    bool found = false;

    for (const auto& a : appointments) {
        if (a.getPatientId() == id.toStdString()) {
            result += "Date: " + QString::fromStdString(a.getDate()) +
                      ", Time: " + QString::fromStdString(a.getTime()) + "\n";
            found = true;
        }
    }

    if (!found) result += "No appointments found.";
    QMessageBox::information(this, "Appointments", result);
}

void PatientWindow::on_pushButton_4_clicked()
{
    bool ok1, ok2;
    QString id = QInputDialog::getText(this, "Cancel Appointment", "Enter Patient ID:", QLineEdit::Normal, "", &ok1);
    QString date = QInputDialog::getText(this, "Cancel Appointment", "Enter Date (DD/MM/YYYY):", QLineEdit::Normal, "", &ok2);
    if (!ok1 || !ok2 || id.isEmpty() || date.isEmpty()) return;

    // استخدام iterator لحذف جميع المواعيد المطابقة
    bool found = false;
    for (auto it = appointments.begin(); it != appointments.end(); ) {
        if (it->getPatientId() == id.toStdString() && it->getDate() == date.toStdString()) {
            it = appointments.erase(it);
            found = true;
        } else {
            ++it;
        }
    }

    if (found) {
        saveData(); // سيحفظ التغييرات تلقائياً في جميع الملفات
        QMessageBox::information(this, "Success", "Appointment(s) cancelled.");
    } else {
        QMessageBox::warning(this, "Error", "Appointment not found.");
    }
}
void PatientWindow::on_pushButton_5_clicked()
{
    bool ok1, ok2, ok3, ok4;
    QString id = QInputDialog::getText(this, "Update Appointment", "Enter Patient ID:", QLineEdit::Normal, "", &ok1);
    QString oldDate = QInputDialog::getText(this, "Update Appointment", "Enter Old Date (DD/MM/YYYY):", QLineEdit::Normal, "", &ok2);
    QString newDate = QInputDialog::getText(this, "Update Appointment", "Enter New Date (DD/MM/YYYY):", QLineEdit::Normal, "", &ok3);
    QString newTime = QInputDialog::getText(this, "Update Appointment", "Enter New Time (HH:MM):", QLineEdit::Normal, "", &ok4);

    if (!ok1 || !ok2 || !ok3 || !ok4 || id.isEmpty() || oldDate.isEmpty() || newDate.isEmpty() || newTime.isEmpty()) return;

    if (!isValidDate(newDate)) {
        QMessageBox::warning(this, "Invalid Date", "Date must be in the format DD/MM/YYYY.");
        return;
    }

    if (!isValidTime(newTime)) {
        QMessageBox::warning(this, "Invalid Time", "Time must be in the format HH:MM.");
        return;
    }

    for (const auto& a : appointments) {
        if (a.getDate() == newDate.toStdString() && a.getTime() == newTime.toStdString()) {
            QMessageBox::warning(this, "Error", "This time slot is already booked.");
            return;
        }
    }

    bool updated = false;
    for (auto& a : appointments) {
        if (a.getPatientId() == id.toStdString() && a.getDate() == oldDate.toStdString()) {
            a.setDate(newDate.toStdString());
            a.setTime(newTime.toStdString());
            updated = true;
            break;
        }
    }

    if (updated) {
        saveData();
        QMessageBox::information(this, "Success", "Appointment updated.");
    } else {
        QMessageBox::warning(this, "Error", "Original appointment not found.");
    }
}

void PatientWindow::on_pushButton_6_clicked()
{
    this->close();
}
