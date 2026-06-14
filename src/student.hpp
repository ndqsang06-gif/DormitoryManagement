#pragma once

#include "config.hpp"
#include "libs/algorithms.hpp"

/*
────────────────────────────────────────────────────────────────────────────────
Find student
────────────────────────────────────────────────────────────────────────────────
*/
/**
 * @brief  find student by studentId
 * @param  studentId: id of student to find
 * @return lower bound index of student in studentsList
 */
size_t findStudent(const std::string& studentId) {
    auto it = binarySearch(studentsList, studentId,
                           [](const Student& student, const std::string& id) {
                               return student.id < id;
                           });

    return it - studentsList.begin();
}

/*
────────────────────────────────────────────────────────────────────────────────
Student management
────────────────────────────────────────────────────────────────────────────────
*/
/**
 * @brief  add student to the studentsList
 * @param  newStudent: student to add
 */
void addStudent(const Student& newStudent) {
    auto idx = findStudent(newStudent.id);

    if (idx != studentsList.size() && studentsList[idx].id == newStudent.id) {
        return;
    }

    studentsList.insert(idx, newStudent);
}

/**
 * @brief  remove student from the studentsList
 * @param  studentId: id of student to remove
 */
void removeStudent(const std::string& studentId) {
    size_t idx = findStudent(studentId);

    if (idx == studentsList.size() || studentsList[idx].id != studentId) {
        return;
    }

    studentsList.erase_at(idx);
}

/**
 * @brief  update student information
 * @param  studentId    : id of student to update
 * @param  newClass     : new class of student
 * @param  newIsPriority: new priority status of student
 * @param  newPhone     : new phone number of student
 * @param  newEmail     : new email of student
 */
void updateStudent(const std::string& studentId, std::string newClass,
                   bool newIsPriority, std::string newPhone,
                   std::string newEmail) {
    size_t idx = findStudent(studentId);

    if (idx == studentsList.size() || studentsList[idx].id != studentId) {
        return;
    }

    Student& student     = studentsList[idx];
    student.studentClass = newClass;
    student.isPriority   = newIsPriority;
    student.phone        = newPhone;
    student.email        = newEmail;
}
