#pragma once

#include "config.hpp"
#include "libs/algorithms.hpp"

/*
────────────────────────────────────────────────────────────────────────────────
Find room
────────────────────────────────────────────────────────────────────────────────
*/
/**
 * @brief  find room by roomId
 * @param  roomId: id of room to find
 * @return lower bound index of room in roomsList
 */
size_t findRoom(const std::string& roomId) {
    auto it = binarySearch(
        roomsList, roomId,
        [](const Room& room, const std::string& id) { return room.id < id; });

    return it - roomsList.begin();
}

/**
 * @brief  find student in a specific room
 * @param  room       : the room to search in
 * @param  studentId  : id of student to find
 * @return lower bound index of studentId in room.students
 */
size_t findStudentInRoom(const Room& room, const std::string& studentId) {
    auto it = binarySearch(room.students, studentId);

    return it - room.students.begin();
}

/*
────────────────────────────────────────────────────────────────────────────────
Room management
────────────────────────────────────────────────────────────────────────────────
*/
/**
 * @brief  add room to the roomsList
 * @param  roomId   : id of the room to add
 * @param  roomType : type of the room to add
 * @param  roomPrice: price of the room to add
 */
void addRoom(const std::string& roomId, size_t roomType, double roomPrice) {
    size_t idx = findRoom(roomId);
    if (idx != roomsList.size() && roomsList[idx].id == roomId) {
        return;
    }
    if (roomType != 4 && roomType != 6 && roomType != 8) {
        return;
    }
    if (roomPrice <= 0) {
        return;
    }

    Room newRoom;
    newRoom.id    = roomId;
    newRoom.type  = roomType;
    newRoom.price = roomPrice;

    roomsList.insert(idx, newRoom);
}

/**
 * @brief  remove room from the roomsList
 * @param  roomId: id of the room to remove
 */
void removeRoom(const std::string& roomId) {
    size_t idx;
    for(idx = 0; idx < roomsList.size(); idx++){
        if(roomsList[idx].id == roomId)
            break;
    }
    if (idx == roomsList.size() || roomsList[idx].id != roomId) {
        return;
    }
    if (roomsList[idx].students.empty() == false) {
        return;
    }

    roomsList.erase_at(idx);
}

/**
 * @brief  update room information
 * @param  roomId  : id of the room to update
 * @param  newType : new type of the room
 * @param  newPrice: new price of the room
 */
void updateRoom(const std::string& roomId, size_t newType, double newPrice) {
    size_t idx = findRoom(roomId);

    if (idx == roomsList.size() || roomsList[idx].id != roomId) {
        return;
    }
    if (newType != 4 && newType != 6 && newType != 8) {
        return;
    }
    if (roomsList[idx].currentStudents() > newType) {
        return;
    }
    if (newPrice <= 0) {
        return;
    }

    roomsList[idx].type  = newType;
    roomsList[idx].price = newPrice;
}

/**
 * @brief  add student to a specific room
 * @param  roomId   : id of the room to add student to
 * @param  studentId: id of the student to add
 */
void addStudentToRoom(const std::string& roomId, const std::string& studentId) {
    size_t roomIdx = findRoom(roomId);
    if (roomIdx == roomsList.size() || roomsList[roomIdx].id != roomId) {
        return;
    }
    if (roomsList[roomIdx].hasAvailableSlot() == false) {
        return;
    }
    auto& room = roomsList[roomIdx];

    size_t studentIdx = findStudentInRoom(room, studentId);
    if (studentIdx != room.students.size() &&
        room.students[studentIdx] == studentId) {
        return;
    }

    room.students.insert(studentIdx, studentId);
}

/**
 * @brief  remove student from a specific room
 * @param  roomId   : id of the room to remove student from
 * @param  studentId: id of the student to remove
 */
void removeStudentFromRoom(const std::string& roomId,
                           const std::string& studentId) {
    size_t roomIdx = findRoom(roomId);
    if (roomIdx == roomsList.size() || roomsList[roomIdx].id != roomId) {
        return;
    }
    auto& room = roomsList[roomIdx];

    size_t studentIdx = findStudentInRoom(room, studentId);
    if (studentIdx == room.students.size() ||
        room.students[studentIdx] != studentId) {
        return;
    }

    room.students.erase_at(studentIdx);
}
