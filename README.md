# 🏥 Clinic Management System – C++ with OOP & File Handling

This is a **Clinic Management System** built entirely in **C++**, implementing **Object-Oriented Programming (OOP)** principles and focusing on **appointment management**, **patient record keeping**, and **report generation**. The system runs in a **console interface**, and it is being extended with a **Qt-based GUI** for enhanced usability.

---

## 🚀 Features

- **Patient Registration** with auto-generated unique ID.
- **Appointment Booking, Removal, Updating, and Viewing**.
- **Conflict Checking** to prevent overlapping appointments.
- **Doctor Section** (with login) to:
  - Generate a full patient report (name, ID, medical history, all appointments).
  - Generate appointment report by a specific date.
- **Data Persistence** using **CSV** for patient and appointment storage.
- **Reports** are generated as readable `.txt` files.
- **Polymorphism** used via abstract `Report` class (for Patient & Appointment reports).

---

## 💡 Technical Overview

- **Class Design**:
  - `Person` (base class) → `Patient` (derived)
  - `Appointment` class handles time and date for visits.
  - `Doctor`, `Menu`, and `FileHandler` handle login, UI, and data loading/saving respectively.
- **OOP Concepts**:
  - **Encapsulation**: Data and behaviors are bundled in classes.
  - **Inheritance**: `Patient` inherits from `Person`.
  - **Polymorphism**: Abstract base class `Report` enables flexible report generation.
- **File Handling**:
  - Loads data from `patients_data/all_patients_data.csv` at startup.
  - Saves updated data back into the same file on exit.
  - Separate `.txt` reports are generated on demand.

---
