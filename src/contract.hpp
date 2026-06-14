#pragma once

#include "config.hpp"
#include "libs/algorithms.hpp"
#include "room.hpp"
#include <string>

/*
────────────────────────────────────────────────────────────────────────────────
Generate contract ID
────────────────────────────────────────────────────────────────────────────────
*/
std::string generateContractId() {
    size_t maxIdx = 0;
    if (!contractsList.empty()) {
        auto lastContract = contractsList.back();
        maxIdx            = std::stoull(lastContract.id.substr(2));
    }

    int    digitNumber = 0;
    size_t nextIdx     = maxIdx + 1;
    while (nextIdx > 0) {
        nextIdx /= 10;
        digitNumber++;
    }
    std::string newId = "CT";
    for (int i = 0; i < 8 - digitNumber; i++) {
        newId += "0";
    }
    newId += std::to_string(maxIdx + 1);

    return newId;
}

/*
────────────────────────────────────────────────────────────────────────────────
Find contract
────────────────────────────────────────────────────────────────────────────────
*/
/**
 * @brief  find contract by contractId
 * @param  contractId: id of contract to find
 * @return lower bound of contractId in contractsList
 */
size_t findContract(const std::string& contractId) {
    auto it = binarySearch(contractsList, contractId,
                           [](const Contract& contract, const std::string& id) {
                               return contract.id < id;
                           });

    return it - contractsList.begin();
}

/**
 * @brief  find active contract of a student
 * @param  studentId: id of student to find
 * @return index of contract if found, contractsList.size() otherwise
 */
size_t findActiveContractOfStudent(const std::string& studentId) {
    for (auto& contract : contractsList) {
        if (contract.studentId == studentId && contract.isActive == true) {
            return &contract - contractsList.begin();
        }
    }
    return contractsList.size();
}

/*
────────────────────────────────────────────────────────────────────────────────
Contract management
────────────────────────────────────────────────────────────────────────────────
*/
/**
 * @brief  add new contract to contractsList
 * @param  studentId    : id of student associated with the contract
 * @param  roomId       : id of room associated with the contract
 * @param  startDate    : start date of the contract
 * @param  endDate      : end date of the contract
 */
void addContract(const std::string& studentId, const std::string& roomId,
                 const base::Date& startDate, const base::Date& endDate) {
    size_t contractIdx = findActiveContractOfStudent(studentId);
    if (contractIdx != contractsList.size()) {
        return;
    }

    size_t roomIdx = findRoom(roomId);
    if (roomIdx == roomsList.size() || roomsList[roomIdx].id != roomId) {
        return;
    }
    if (startDate >= endDate) {
        return;
    }

    Contract newContract;

    newContract.id        = generateContractId();
    newContract.studentId = studentId;
    newContract.roomId    = roomId;
    newContract.startDate = startDate;
    newContract.endDate   = endDate;
    newContract.isActive  = true;

    contractsList.push_back(newContract);
}

/**
 * @brief  remove contract from contractsList
 * @param  contractId: id of contract to remove
 */
void removeContract(const std::string& contractId) {
    size_t idx = findContract(contractId);

    if (idx == contractsList.size() || contractsList[idx].id != contractId) {
        return;
    }
    if (contractsList[idx].isActive == true) {
        return;
    }

    contractsList.erase_at(idx);
}

/**
 * @brief  extend end date of a contract
 * @param  contractId: id of contract to extend
 * @param  newEndDate: new end date of the contract
 */
void extendContract(const std::string& contractId,
                    const base::Date&  newEndDate) {
    size_t idx = findContract(contractId);

    if (idx == contractsList.size() || contractsList[idx].id != contractId) {
        return;
    }
    if (contractsList[idx].isActive == false) {

        return;
    }
    if (contractsList[idx].endDate >= newEndDate) {
        return;
    }

    contractsList[idx].endDate = newEndDate;
}

/**
 * @brief  terminate a contract
 * @param  contractId: id of contract to terminate
 */
void terminateContract(const std::string& contractId) {
    size_t idx = findContract(contractId);

    if (idx == contractsList.size() || contractsList[idx].id != contractId) {
        return;
    }
    if (contractsList[idx].isActive == false) {
        return;
    }

    contractsList[idx].isActive = false;
}

/*
────────────────────────────────────────────────────────────────────────────────
Room assignment management
────────────────────────────────────────────────────────────────────────────────
*/
/**
 * @brief  assign student to a room
 * @param  studentId    : id of student to assign
 * @param  roomId       : id of room to assign student to
 * @param  startDate    : start date of the assignment
 * @param  endDate      : end date of the assignment
 */
void registerRoom(const std::string& studentId, const std::string& roomId,
                  const base::Date& startDate, const base::Date& endDate) {
    size_t contractIdx = findActiveContractOfStudent(studentId);
    if (contractIdx != contractsList.size()) {
        return;
    }

    size_t roomIdx = findRoom(roomId);
    if (roomIdx == roomsList.size() || roomsList[roomIdx].id != roomId) {
        return;
    }
    if (roomsList[roomIdx].hasAvailableSlot() == false) {
        return;
    }

    if (startDate >= endDate) {
        return;
    }

    addStudentToRoom(roomId, studentId);
    addContract(studentId, roomId, startDate, endDate);
}

/**
 * @brief  transfer student to a different room
 * @param  studentId    : id of student to transfer
 * @param  newRoomId    : id of new room to transfer student to
 * @param  newStartDate : start date of the new contract
 * @param  newEndDate   : end date of the new contract
 */
void transferRoom(const std::string& studentId, const std::string& newRoomId,
                  const base::Date& newStartDate,
                  const base::Date& newEndDate) {

    size_t oldContractIdx = findActiveContractOfStudent(studentId);
    if (oldContractIdx == contractsList.size()) {
        return;
    }
    std::string oldRoomId     = contractsList[oldContractIdx].roomId;
    std::string oldContractId = contractsList[oldContractIdx].id;

    if (oldRoomId == newRoomId) {
        return;
    }
    if (contractsList[oldContractIdx].isActive == false) {
        return;
    }

    size_t newRoomIdx = findRoom(newRoomId);
    if (newRoomIdx == roomsList.size() ||
        roomsList[newRoomIdx].id != newRoomId) {
        return;
    }
    if (roomsList[newRoomIdx].hasAvailableSlot() == false) {
        return;
    }

    if (newStartDate >= newEndDate) {
        return;
    }

    removeStudentFromRoom(oldRoomId, studentId);
    terminateContract(oldContractId);
    registerRoom(studentId, newRoomId, newStartDate, newEndDate);
}

/**
 * @brief  checkout student from a room
 * @param  studentId: id of student to checkout
 */
void checkoutRoom(const std::string& studentId) {
    size_t idx = findActiveContractOfStudent(studentId);

    if (idx == contractsList.size()) {
        return;
    }
    if (contractsList[idx].isActive == false) {
        return;
    }

    std::string contractId = contractsList[idx].id;
    std::string roomId     = contractsList[idx].roomId;

    removeStudentFromRoom(roomId, studentId);
    terminateContract(contractId);
}
