#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

struct Student {
    int rollNumber{};
    std::string name;
    int age{};
    std::string course;
    double marks{};
};

const std::string DATA_FILE = "students.txt";

// Read a complete line and accept it only when it contains one valid integer.
int readInteger(const std::string& prompt, int minimum, int maximum) {
    while (true) {
        std::cout << prompt;
        std::string line;
        std::getline(std::cin, line);

        std::istringstream input(line);
        int value{};
        char extra{};
        if ((input >> value) && !(input >> extra) && value >= minimum && value <= maximum) {
            return value;
        }

        std::cout << "Invalid input. Enter a whole number from " << minimum
                  << " to " << maximum << ".\n";
    }
}

double readMarks(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string line;
        std::getline(std::cin, line);

        std::istringstream input(line);
        double value{};
        char extra{};
        if ((input >> value) && !(input >> extra) && std::isfinite(value) &&
            value >= 0.0 && value <= 100.0) {
            return value;
        }

        std::cout << "Invalid input. Enter marks between 0 and 100.\n";
    }
}

std::string readNonEmptyText(const std::string& prompt) {
    while (true) {
        std::cout << prompt;
        std::string value;
        std::getline(std::cin, value);

        const std::size_t first = value.find_first_not_of(" \t\r\n");
        if (first != std::string::npos) {
            const std::size_t last = value.find_last_not_of(" \t\r\n");
            return value.substr(first, last - first + 1);
        }
        std::cout << "This field cannot be empty. Please try again.\n";
    }
}

bool loadStudents(std::vector<Student>& students) {
    std::ifstream file(DATA_FILE);
    if (!file) {
        // A missing data file is normal on the first run.
        return true;
    }

    Student student;
    while (file >> student.rollNumber >> std::quoted(student.name) >> student.age
                >> std::quoted(student.course) >> student.marks) {
        students.push_back(student);
    }

    if (!file.eof()) {
        std::cerr << "Error: " << DATA_FILE
                  << " contains invalid or incomplete data. No changes were made.\n";
        return false;
    }
    return true;
}

bool saveStudents(const std::vector<Student>& students) {
    std::ofstream file(DATA_FILE, std::ios::trunc);
    if (!file) {
        std::cerr << "Error: Could not open " << DATA_FILE << " for writing.\n";
        return false;
    }

    file << std::setprecision(10);
    for (const Student& student : students) {
        file << student.rollNumber << ' ' << std::quoted(student.name) << ' '
             << student.age << ' ' << std::quoted(student.course) << ' '
             << student.marks << '\n';
    }

    if (!file) {
        std::cerr << "Error: Failed while saving student records.\n";
        return false;
    }
    return true;
}

int findStudentIndex(const std::vector<Student>& students, int rollNumber) {
    for (std::size_t i = 0; i < students.size(); ++i) {
        if (students[i].rollNumber == rollNumber) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

void addStudent(std::vector<Student>& students) {
    std::cout << "\n--- Add Student ---\n";
    const int rollNumber = readInteger("Roll number: ", 1, std::numeric_limits<int>::max());

    if (findStudentIndex(students, rollNumber) != -1) {
        std::cout << "A student with that roll number already exists.\n";
        return;
    }

    Student student;
    student.rollNumber = rollNumber;
    student.name = readNonEmptyText("Name: ");
    student.age = readInteger("Age: ", 1, 120);
    student.course = readNonEmptyText("Course: ");
    student.marks = readMarks("Marks (0-100): ");

    students.push_back(student);
    if (saveStudents(students)) {
        std::cout << "Student added successfully.\n";
    } else {
        students.pop_back();
    }
}

void displayStudent(const Student& student) {
    std::cout << std::left
              << std::setw(12) << student.rollNumber
              << std::setw(26) << student.name
              << std::setw(8) << student.age
              << std::setw(22) << student.course
              << std::fixed << std::setprecision(2) << student.marks << '\n';
}

void displayAllStudents(const std::vector<Student>& students) {
    std::cout << "\n--- Student Records ---\n";
    if (students.empty()) {
        std::cout << "No student records found.\n";
        return;
    }

    std::cout << std::left
              << std::setw(12) << "Roll No."
              << std::setw(26) << "Name"
              << std::setw(8) << "Age"
              << std::setw(22) << "Course"
              << "Marks\n";
    std::cout << std::string(78, '-') << '\n';

    for (const Student& student : students) {
        displayStudent(student);
    }
}

void updateStudent(std::vector<Student>& students) {
    std::cout << "\n--- Update Student ---\n";
    const int rollNumber = readInteger("Enter roll number to update: ", 1,
                                       std::numeric_limits<int>::max());
    const int index = findStudentIndex(students, rollNumber);

    if (index == -1) {
        std::cout << "Student not found.\n";
        return;
    }

    std::cout << "Current record:\n";
    displayAllStudents(std::vector<Student>{students[static_cast<std::size_t>(index)]});
    std::cout << "Enter the updated details (the roll number will remain unchanged).\n";

    Student updated = students[static_cast<std::size_t>(index)];
    updated.name = readNonEmptyText("New name: ");
    updated.age = readInteger("New age: ", 1, 120);
    updated.course = readNonEmptyText("New course: ");
    updated.marks = readMarks("New marks (0-100): ");

    Student previous = students[static_cast<std::size_t>(index)];
    students[static_cast<std::size_t>(index)] = updated;
    if (saveStudents(students)) {
        std::cout << "Student updated successfully.\n";
    } else {
        students[static_cast<std::size_t>(index)] = previous;
    }
}

void deleteStudent(std::vector<Student>& students) {
    std::cout << "\n--- Delete Student ---\n";
    const int rollNumber = readInteger("Enter roll number to delete: ", 1,
                                       std::numeric_limits<int>::max());
    const int index = findStudentIndex(students, rollNumber);

    if (index == -1) {
        std::cout << "Student not found.\n";
        return;
    }

    std::cout << "Record to be deleted:\n";
    displayAllStudents(std::vector<Student>{students[static_cast<std::size_t>(index)]});
    std::cout << "Delete this record? (y/n): ";
    std::string answer;
    std::getline(std::cin, answer);

    if (answer != "y" && answer != "Y") {
        std::cout << "Deletion cancelled.\n";
        return;
    }

    const std::size_t position = static_cast<std::size_t>(index);
    Student removed = students[position];
    students.erase(students.begin() + index);
    if (saveStudents(students)) {
        std::cout << "Student deleted successfully.\n";
    } else {
        students.insert(students.begin() + static_cast<std::ptrdiff_t>(position), removed);
    }
}

void showMenu() {
    std::cout << "\n========== STUDENT MANAGEMENT SYSTEM ==========\n"
              << "1. Add Student\n"
              << "2. Update Student\n"
              << "3. Delete Student\n"
              << "4. Display All Students\n"
              << "5. Exit\n"
              << "================================================\n";
}

int main() {
    std::vector<Student> students;
    if (!loadStudents(students)) {
        return 1;
    }

    std::cout << "Student Management System\n";
    std::cout << students.size() << " record(s) loaded from " << DATA_FILE << ".\n";

    while (true) {
        showMenu();
        const int choice = readInteger("Enter your choice (1-5): ", 1, 5);

        switch (choice) {
            case 1:
                addStudent(students);
                break;
            case 2:
                updateStudent(students);
                break;
            case 3:
                deleteStudent(students);
                break;
            case 4:
                displayAllStudents(students);
                break;
            case 5:
                std::cout << "Thank you for using the Student Management System.\n";
                return 0;
            default:
                break; // The input helper already restricts the choice to 1-5.
        }
    }
}
