#pragma once

#include "config.hpp"
#include "libs/vector.hpp"
#include <string>

/**
 * @brief  Finds service invoices that are overdue by at least one month.
 * @param  currentDate : Current date in "YYYY/MM/DD" format.
 * @return A vector of pointers to overdue ServiceInvoice objects in ServiceInvoicesList.
 */
base::Vector<ServiceInvoice*>
findOverdueInvoices(const std::string& currentDate) {
    base::Vector<ServiceInvoice*> reportList;

    int currentYear  = std::stoi(currentDate.substr(0, 4));
    int currentMonth = std::stoi(currentDate.substr(5, 2));

    for (auto& invoice : serviceInvoicesList) {
        if (!invoice.isPaid) {
            int monthsDiff = (currentYear - invoice.year) * 12 +
                             (currentMonth - invoice.month);

            if (monthsDiff >= 1) {
                reportList.push_back(&invoice);
            }
        }
    }
    return reportList;
}


/**
 * @struct OccupancyStatus
 * @brief  Represents the occupancy status of the dormitory.
 */
struct OccupancyStatus {
    size_t rentedRooms;   
    size_t emptyRooms;    
    double occupancyRate; 
};

/**
 * @brief  Calculates the occupancy status of the dormitory.
 * @return An OccupancyStatus struct containing rented rooms, empty rooms, and occupancy rate.
 */
OccupancyStatus createOccupancyReport() {
    OccupancyStatus status     = {0, 0, 0.0};
    size_t          totalRooms = roomsList.size();

    for (const auto& room : roomsList) {
        if (room.currentStudents() > 0) {
            status.rentedRooms++;
        } else {
            status.emptyRooms++;
        }
    }

    if (totalRooms > 0) {
        status.occupancyRate =
            (static_cast<double>(status.rentedRooms) / totalRooms) * 100.0;
    }

    return status;
}


/**
 * @brief  Finds contracts that are about to expire within a specified date range.
 * @param  currentDate : Current date in "YYYY/MM/DD" format.
 * @param  limitDate   : Limit date for expiration in "YYYY/MM/DD" format.
 * @return A vector of pointers to expiring Contract objects in ContractsList.
 */
base::Vector<Contract*> findExpiringContracts(const std::string& currentDate,
                                              const std::string& limitDate) {
    base::Vector<Contract*> reportList;

    for (auto& contract : contractsList) {
        if (!contract.isActive)
            continue;

        // Lexicographical comparison for "YYYY-MM-DD" formatted strings
        if (contract.endDate >= currentDate &&
            contract.endDate <= limitDate) {
            reportList.push_back(&contract);
        }
    }
    return reportList;
}
