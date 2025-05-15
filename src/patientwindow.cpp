#include "patientwindow.h"
#include "ui_patientwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include <algorithm>
#include <QFile>
#include <QTextStream>
#include <regex>

#include "patient.h"
#include "appointment.h"
#include "global.h"
Patient::Patient(const std::string& name, const std::string& id, const std::string& history)
    : name(name), id(id), history(history) {}

std::string Patient::getName() const { return name; }
std::string Patient::getId() const { return id; }
std::string Patient::getHistory() const { return history; }

QJsonObject Patient::toJson() const {
    QJsonObject json;
    json["name"] = QString::fromStdString(name);
    json["id"] = QString::fromStdString(id);
    json["history"] = QString::fromStdString(history);
    return json;
}

Patient Patient::fromJson(const QJsonObject& json) {
    return Patient(
        json["name"].toString().toStdString(),
        json["id"].toString().toStdString(),
        json["history"].toString().toStdString()
        );
}

Appointment::Appointment(const std::string& patientId, const std::string& date, const std::string& time)
    : patientId(patientId), date(date), time(time) {}

std::string Appointment::getPatientId() const { return patientId; }
std::string Appointment::getDate() const { return date; }
std::string Appointment::getTime() const { return time; }
void Appointment::setDate(const std::string& newDate) { date = newDate; }
void Appointment::setTime(const std::string& newTime) { time = newTime; }

QJsonObject Appointment::toJson() const {
    QJsonObject json;
    json["patientId"] = QString::fromStdString(patientId);
    json["date"] = QString::fromStdString(date);
    json["time"] = QString::fromStdString(time);
    return json;
}

Appointment Appointment::fromJson(const QJsonObject& json) {
    return Appointment(
        json["patientId"].toString().toStdString(),
        json["date"].toString().toStdString(),
        json["time"].toString().toStdString()
        );
}

PatientWindow::PatientWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PatientWindow)
{
    ui->setupUi(this);
    loadData();
}

PatientWindow::~PatientWindow() {
    delete ui;
    saveData();
}

void PatientWindow::loadData() {
    QFile file("data.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open data file.");
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject json = doc.object();

    QJsonArray patientsArray = json["patients"].toArray();
    for (const QJsonValue& value : patientsArray) {
        patients.push_back(Patient::fromJson(value.toObject()));
        lastPatientId = std::max(lastPatientId, value.toObject()["id"].toString().toInt());
    }

    QJsonArray appointmentsArray = json["appointments"].toArray();
    for (const QJsonValue& value : appointmentsArray) {
        appointments.push_back(Appointment::fromJson(value.toObject()));
    }

    file.close();
}

void PatientWindow::saveData() {
    QJsonObject json;

    QJsonArray patientsArray;
    for (const Patient& p : patients) {
        patientsArray.append(p.toJson());
    }
    json["patients"] = patientsArray;

    QJsonArray appointmentsArray;
    for (const Appointment& a : appointments) {
        appointmentsArray.append(a.toJson());
    }
    json["appointments"] = appointmentsArray;

    QFile file("data.json");
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning("Couldn't open data file for writing.");
        return;
    }

    file.write(QJsonDocument(json).toJson());
    file.close();
}

bool PatientWindow::isOnlyLetters(const std::string& str) {
    return std::regex_match(str, std::regex("^[A-Za-z ]+$"));
}

bool PatientWindow::isValidDate(const QString& date) {
    QDate d = QDate::fromString(date, "dd/MM/yyyy");
    return d.isValid() && d >= QDate::currentDate();
}

bool PatientWindow::isValidTime(const QString& time) {
    return QTime::fromString(time, "HH:mm").isValid();
}

bool PatientWindow::patientExists(const QString& id) {
    return std::any_of(patients.begin(), patients.end(), [&](const Patient& p) {
        return p.getId() == id.toStdString();
    });
}

bool PatientWindow::appointmentExists(const QString& date, const QString& time) {
    return std::any_of(appointments.begin(), appointments.end(), [&](const Appointment& a) {
        return a.getDate() == date.toStdString() && a.getTime() == time.toStdString();
    });
}

void PatientWindow::on_pushButton_clicked() {
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
    patients.emplace_back(name.toStdString(), id.toStdString(), history.toStdString());

    saveData();
    QMessageBox::information(this, "Success", "Patient added with ID: " + id);
}

void PatientWindow::on_pushButton_2_clicked() {
    bool ok1, ok2, ok3;
    QString id = QInputDialog::getText(this, "Book Appointment", "Enter Patient ID:", QLineEdit::Normal, "", &ok1);
    if (!ok1 || id.isEmpty()) return;

    if (!patientExists(id)) {
        QMessageBox::warning(this, "Error", "Patient ID not found.");
        return;
    }

    QString date = QInputDialog::getText(this, "Book Appointment", "Enter Date (DD/MM/YYYY):", QLineEdit::Normal, "", &ok2);
    if (!ok2 || date.isEmpty() || !isValidDate(date)) {
        QMessageBox::warning(this, "Error", "Invalid date format or past date!");
        return;
    }

    QString time = QInputDialog::getText(this, "Book Appointment", "Enter Time (HH:MM):", QLineEdit::Normal, "", &ok3);
    if (!ok3 || time.isEmpty() || !isValidTime(time)) {
        QMessageBox::warning(this, "Error", "Invalid time format!");
        return;
    }

    if (appointmentExists(date, time)) {
        QMessageBox::warning(this, "Error", "This time slot is already booked.");
        return;
    }

    appointments.emplace_back(id.toStdString(), date.toStdString(), time.toStdString());
    saveData();
    QMessageBox::information(this, "Success", "Appointment booked successfully.");
}

void PatientWindow::on_pushButton_3_clicked() {
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

void PatientWindow::on_pushButton_4_clicked() {
    bool ok1, ok2;
    QString id = QInputDialog::getText(this, "Cancel Appointment", "Enter Patient ID:", QLineEdit::Normal, "", &ok1);
    QString date = QInputDialog::getText(this, "Cancel Appointment", "Enter Date (DD/MM/YYYY):", QLineEdit::Normal, "", &ok2);
    if (!ok1 || !ok2 || id.isEmpty() || date.isEmpty()) return;

    auto it = std::remove_if(appointments.begin(), appointments.end(),
                             [&](const Appointment& a) {
                                 return a.getPatientId() == id.toStdString() &&
                                        a.getDate() == date.toStdString();
                             });

    if (it != appointments.end()) {
        appointments.erase(it, appointments.end());
        saveData();
        QMessageBox::information(this, "Success", "Appointment cancelled.");
    } else {
        QMessageBox::warning(this, "Error", "Appointment not found.");
    }
}

void PatientWindow::on_pushButton_5_clicked() {
    bool ok1, ok2, ok3, ok4;
    QString id = QInputDialog::getText(this, "Update Appointment", "Enter Patient ID:", QLineEdit::Normal, "", &ok1);
    QString oldDate = QInputDialog::getText(this, "Update Appointment", "Enter Old Date (DD/MM/YYYY):", QLineEdit::Normal, "", &ok2);
    QString newDate = QInputDialog::getText(this, "Update Appointment", "Enter New Date (DD/MM/YYYY):", QLineEdit::Normal, "", &ok3);
    QString newTime = QInputDialog::getText(this, "Update Appointment", "Enter New Time (HH:MM):", QLineEdit::Normal, "", &ok4);

    if (!ok1 || !ok2 || !ok3 || !ok4 || id.isEmpty() || oldDate.isEmpty() || newDate.isEmpty() || newTime.isEmpty())
        return;

    if (!isValidDate(newDate) || !isValidTime(newTime)) {
        QMessageBox::warning(this, "Error", "Invalid date or time format!");
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

/*void PatientWindow::on_pushButton_6_clicked()
{

}


void PatientWindow::on_pushButton_7_clicked()
{

}*/

