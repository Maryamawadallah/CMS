#ifndef GLOBALS_H
#define GLOBALS_H

#include <vector>
#include <string>
#include "patient.h"
#include "appointment.h"

// تعريف المتغيرات العالمية
extern std::vector<Patient> patients;
extern std::vector<Appointment> appointments;
extern int lastPatientId;

#endif // GLOBALS_H
