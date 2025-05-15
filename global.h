#ifndef GLOBAL_H
#define GLOBAL_H

#include <vector>
#include "patient.h"
#include "appointment.h"

extern std::vector<Patient> patients;
extern std::vector<Appointment> appointments;
extern int lastPatientId;

#endif // GLOBAL_H
