#include "patient.h"

Patient::Patient(const std::string& name, const std::string& id, const std::string& history)
    : name(name), id(id), history(history) {}

std::string Patient::getName() const { return name; }
std::string Patient::getId() const { return id; }
std::string Patient::getHistory() const { return history; }
