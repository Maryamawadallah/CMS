#include "menudoc.h"
#include "ui_menudoc.h"
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QInputDialog>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

MenuDoc::MenuDoc(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MenuDoc)
{
    ui->setupUi(this);
}

MenuDoc::~MenuDoc()
{
    delete ui;
}

void MenuDoc::on_pushButton_view_clicked() {
    bool ok;
    QString date = QInputDialog::getText(this, "View Appointments", "Enter date (DD/MM/YYYY):", QLineEdit::Normal, "", &ok);

    if (ok && !date.isEmpty()) {
        QFile file("data.json");
        if (!file.open(QIODevice::ReadOnly)) {
            QMessageBox::warning(this, "Error", "Could not open data file");
            return;
        }

        QByteArray data = file.readAll();
        QJsonDocument doc(QJsonDocument::fromJson(data));
        QJsonObject json = doc.object();
        file.close();

        QStringList matchedAppointments;
        QJsonArray appointments = json["appointments"].toArray();

        for (const QJsonValue& value : appointments) {
            QJsonObject appt = value.toObject();
            if (appt["date"].toString() == date) {
                matchedAppointments << QString("Patient ID: %1, Time: %2")
                .arg(appt["patientId"].toString())
                    .arg(appt["time"].toString());
            }
        }

        if (matchedAppointments.isEmpty()) {
            QMessageBox::information(this, "Appointments", "No appointments found for this date.");
        } else {
            QMessageBox::information(this, "Appointments", matchedAppointments.join("\n"));
        }
    }
}

void MenuDoc::on_pushButton_Report_clicked() {
    QFile file("data.json");
    if (!file.exists()) {
        QMessageBox::information(this, "Patient Report", "No patient data found.");
        return;
    }

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Error", "Could not open data file");
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject json = doc.object();
    file.close();

    QString report = "===== PATIENT REPORT =====\n";
    QJsonArray patients = json["patients"].toArray();
    QJsonArray appointments = json["appointments"].toArray();

    for (const QJsonValue& patientValue : patients) {
        QJsonObject patient = patientValue.toObject();
        report += "Name: " + patient["name"].toString() +
                  ", ID: " + patient["id"].toString() +
                  ", History: " + patient["history"].toString() + "\n";

        for (const QJsonValue& apptValue : appointments) {
            QJsonObject appt = apptValue.toObject();
            if (appt["patientId"].toString() == patient["id"].toString()) {
                report += "   - " + appt["date"].toString() + " at " + appt["time"].toString() + "\n";
            }
        }
    }

    if (patients.isEmpty()) {
        QMessageBox::information(this, "Patient Report", "No patient data found.");
    } else {
        QMessageBox::information(this, "Patient Report", report);
    }
}

void MenuDoc::on_pushButton_Exit_clicked() {
    this->close();
}
