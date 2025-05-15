#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <string>

class Appointment
{
public:
    Appointment(const std::string& patientId, const std::string& date, const std::string& time);

    std::string getPatientId() const;
    std::string getDate() const;
    std::string getTime() const;
    void setDate(const std::string& newDate);
    void setTime(const std::string& newTime);

private:
    std::string patientId;
    std::string date;
    std::string time;
};

#endif // APPOINTMENT_H
