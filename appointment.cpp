#include "appointment.h"

Appointment::Appointment(const std::string& patientId, const std::string& date, const std::string& time)
    : patientId(patientId), date(date), time(time) {}

std::string Appointment::getPatientId() const { return patientId; }
std::string Appointment::getDate() const { return date; }
std::string Appointment::getTime() const { return time; }

void Appointment::setDate(const std::string& newDate) { date = newDate; }
void Appointment::setTime(const std::string& newTime) { time = newTime; }
