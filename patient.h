#ifndef PATIENT_H
#define PATIENT_H

#include <string>

class Patient
{
public:
    Patient(const std::string& name, const std::string& id, const std::string& history);

    std::string getName() const;
    std::string getId() const;
    std::string getHistory() const;

private:
    std::string name;
    std::string id;
    std::string history;
};

#endif // PATIENT_H
