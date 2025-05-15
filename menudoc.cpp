#include "menudoc.h"
#include "ui_menudoc.h"
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QInputDialog>
#include <QDir>
#include <QDate>
#include <QDateTime>
#include <QDebug>

MenuDoc::MenuDoc(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MenuDoc)
{
    ui->setupUi(this);
    QDir().mkpath(QCoreApplication::applicationDirPath() + "/Data");
    createFileIfNotExists(QCoreApplication::applicationDirPath() + "/Data/appointments.csv",
                          "PatientID,Date,Time\n");
    createFileIfNotExists(QCoreApplication::applicationDirPath() + "/Data/reports.txt",
                          "===== REPORTS =====\n");
}

MenuDoc::~MenuDoc()
{
    delete ui;
}

void MenuDoc::createFileIfNotExists(const QString &filename, const QString &header)
{
    QFile file(filename);
    if (!file.exists()) {
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << header;
            file.close();
        }
    }
}

QVector<QStringList> MenuDoc::readAppointmentsData()
{
    QVector<QStringList> data;
    QString filePath = QCoreApplication::applicationDirPath() + "/Data/appointments.csv";
    QFile file(filePath);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        bool firstLine = true;

        while (!in.atEnd()) {
            QString line = in.readLine().trimmed();
            qDebug() << "Read line: " << line;  // نعرض السطور التي تتم قراءتها

            if (line.isEmpty() || line.startsWith("=")) continue;

            // تجاهل السطر الأول (الـ Header)
            if (firstLine) {
                firstLine = false;
                continue;
            }

            QStringList fields = line.split(',');  // استخدام الفاصلة كفاصل
            qDebug() << "Fields: " << fields;  // عرض الحقول المقسمة

            if (fields.size() == 3) {
                QString patientID = fields[0];
                QString date = fields[1];
                QString time = fields[2];

                // التحقق من أن التاريخ والوقت في التنسيق الصحيح
                QDate appointmentDate = QDate::fromString(date, "dd/MM/yyyy");
                if (!appointmentDate.isValid()) {
                    qDebug() << "Invalid date format: " << date;
                    continue;
                }

                // التحقق من أن الوقت في التنسيق الصحيح
                QTime appointmentTime = QTime::fromString(time, "hh:mm");
                if (!appointmentTime.isValid()) {
                    qDebug() << "Invalid time format: " << time;
                    continue;
                }

                // إضافة البيانات الصالحة فقط
                qDebug() << "Valid appointment: " << fields;
                data.append(fields);
            } else {
                qDebug() << "Skipping invalid line: " << line;
            }
        }
        file.close();
    } else {
        showError("Cannot open appointments file!");
    }

    return data;
}

void MenuDoc::showError(const QString &message)
{
    QMessageBox::critical(this, "Error", message);
}

void MenuDoc::writeToReport(const QString &content)
{
    QString reportPath = QCoreApplication::applicationDirPath() + "/Data/reports.txt";
    QFile file(reportPath);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << content;
        file.close();
    } else {
        showError("Cannot write to report file!");
    }
}

void MenuDoc::on_pushButton_Report_clicked()
{
    QVector<QStringList> appointmentsData = readAppointmentsData();

    QString report = "===== REPORTS =====\n";
    report += "\n===== APPOINTMENTS REPORT - " +
              QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm") +
              " =====\n\n";

    if (appointmentsData.isEmpty()) {
        report += "No appointments found in the system.\n";
    } else {
        QMap<QString, QList<QPair<QDate, QTime>>> appointmentsByPatient;

        // تجميع المواعيد حسب رقم المريض
        for (const QStringList &appointment : appointmentsData) {
            QString patientID = appointment[0];  // رقم المريض
            QString date = appointment[1];
            QString time = appointment[2];

            QDate appointmentDate = QDate::fromString(date, "dd/MM/yyyy");
            QTime appointmentTime = QTime::fromString(time, "hh:mm");

            if (appointmentDate.isValid() && appointmentTime.isValid()) {
                appointmentsByPatient[patientID].append(qMakePair(appointmentDate, appointmentTime));
            }
        }

        // ترتيب مواعيد كل مريض حسب التاريخ والوقت
        for (auto &appointments : appointmentsByPatient) {
            std::sort(appointments.begin(), appointments.end(), [](const QPair<QDate, QTime> &a, const QPair<QDate, QTime> &b) {
                if (a.first != b.first)
                    return a.first < b.first;
                return a.second < b.second;
            });
        }

        // ترتيب المرضى حسب رقم المريض (ID)
        QVector<QString> patientIDs = appointmentsByPatient.keys().toVector();
        std::sort(patientIDs.begin(), patientIDs.end(), [](const QString &a, const QString &b) {
            return a.toInt() < b.toInt();
        });

        // توليد التقرير
        for (const QString &patientID : patientIDs) {
            report += "Patient: " + patientID + "\n";
            report += "Appointments:\n";
            for (const auto &appointment : appointmentsByPatient[patientID]) {
                report += "  - " + appointment.first.toString("dd/MM/yyyy") + " at " + appointment.second.toString("hh:mm") + "\n";
            }
            report += "----------------------------\n";
        }
    }

    writeToReport(report);
    QMessageBox::information(this, "Success", "Appointments report generated successfully!");
}

void MenuDoc::on_pushButton_view_clicked()
{
    bool ok;
    QString inputDateStr = QInputDialog::getText(this, "Search Appointments",
                                                 "Enter date (dd/MM/yyyy):",
                                                 QLineEdit::Normal, "", &ok);
    if (!ok || inputDateStr.isEmpty()) return;

    QDate inputDate = QDate::fromString(inputDateStr, "dd/MM/yyyy");
    QVector<QStringList> appointmentsData = readAppointmentsData();

    QString result = "===== APPOINTMENTS ON " + inputDate.toString("dd/MM/yyyy") + " =====\n\n";
    bool found = false;

    for (const QStringList &appointment : appointmentsData) {
        QDate appointmentDate = QDate::fromString(appointment[1], "dd/MM/yyyy");
        if (appointmentDate == inputDate) {
            found = true;
            result += "Patient: " + appointment[0] + "\n";
            result += "Time: " + appointment[2] + "\n";
            result += "----------------------------\n";
        }
    }

    if (!found) {
        result += "No appointments found for this date.\n";
    }

    // تحديد اسم الملف باستخدام التاريخ الحالي
    QString filename = QCoreApplication::applicationDirPath() + "/Data/appointments_" + inputDate.toString("yyyyMMdd") + ".txt";

    // كتابة البيانات في الملف
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << result;
        file.close();
        QMessageBox::information(this, "Success", "Search results saved to file: " + filename);
    } else {
        showError("Cannot save results to file!");
    }
}

void MenuDoc::on_pushButton_Exit_clicked()
{
    this->close();
}
