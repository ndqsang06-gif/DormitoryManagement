#pragma once

#include "config.hpp"
#include "libs/vector.hpp"
#include <string>


/*
────────────────────────────────────────────────────────────────────────────────
Các hàm xử lý Báo Cáo
────────────────────────────────────────────────────────────────────────────────
*/

/**
 * @brief  1. Lấy danh sách hóa đơn dịch vụ nợ phí quá hạn (>= 1 tháng)
 * @param  currentMonth: Tháng hiện tại (được truyền vào hệ thống)
 * @param  currentYear : Năm hiện tại (được truyền vào hệ thống)
 * @return Danh sách các đối tượng ServiceInvoice gốc thỏa mãn điều kiện
 */
base::Vector<ServiceInvoice> getOverdueInvoicesReport(int currentMonth, int currentYear) {
    base::Vector<ServiceInvoice> reportList;

    for (const auto& invoice : serviceInvoicesList) {
        if (!invoice.isPaid) {
            int monthsDiff = (currentYear - invoice.year) * 12 + (currentMonth - invoice.month);

            if (monthsDiff >= 1) {
                reportList.push_back(invoice);
            }
        }
    }
    return reportList;
}

/**
 * @brief  2. Tính toán tỷ lệ lấp đầy
 * @return Struct OccupancyReport chứa tổng hợp các chỉ số lấp đầy
 */
OccupancyReport getOccupancyReport() {
    OccupancyReport report = {0, 0, 0.0, 0, 0, 0.0};

    report.totalRooms = roomsList.size();

    for (const auto& room : roomsList) {
        // Lưu vào biến để không gọi currentStudents() 2 lần
        size_t n = room.currentStudents();

        if (n > 0) {
            report.occupiedRooms++;
        }

        report.totalBeds    += room.type;
        report.occupiedBeds += static_cast<int>(n);
    }

    if (report.totalRooms > 0) {
        report.roomOccupancyRate = (static_cast<double>(report.occupiedRooms) / report.totalRooms) * 100.0;
    }

    if (report.totalBeds > 0) {
        report.bedOccupancyRate = (static_cast<double>(report.occupiedBeds) / report.totalBeds) * 100.0;
    }

    return report;
}

/**
 * @brief  3. Lấy danh sách hợp đồng sắp hết hạn
 * @note   Contract.endDate lưu dạng "YYYY-MM-DD" (theo fileIO.hpp).
 *         Parse thành số nguyên YYYYMMDD bằng std::stoi + substr để so sánh
 *         chính xác theo ngữ nghĩa thời gian.
 * @param  currentYear, currentMonth, currentDay : Ngày hiện tại (truyền vào hệ thống)
 * @param  limitYear,   limitMonth,   limitDay   : Ngày giới hạn cảnh báo (truyền vào hệ thống)
 * @return Danh sách các đối tượng Contract gốc thỏa mãn điều kiện
 */
base::Vector<Contract> getExpiringContractsReport(
    int currentYear,  int currentMonth,  int currentDay,
    int limitYear,    int limitMonth,    int limitDay)
{
    base::Vector<Contract> reportList;

    int currentDateInt = currentYear * 10000 + currentMonth * 100 + currentDay;
    int limitDateInt   = limitYear   * 10000 + limitMonth   * 100 + limitDay;

    for (const auto& contract : contractsList) {
        if (!contract.isActive) continue;

        // Parse "YYYY-MM-DD" -> int YYYYMMDD
        // Dùng std::stoi và substr đã có sẵn trong <string> (fileIO.hpp dùng cùng cách)
        int endDateInt = std::stoi(contract.endDate.substr(0, 4)) * 10000
                       + std::stoi(contract.endDate.substr(5, 2)) * 100
                       + std::stoi(contract.endDate.substr(8, 2));

        if (endDateInt >= currentDateInt && endDateInt <= limitDateInt) {
            reportList.push_back(contract);
        }
    }
    return reportList;
}