#pragma once

#include "config.hpp"
#include "libs/algorithms.hpp"
#include <string>

/*
────────────────────────────────────────────────────────────────────────────────
Generate id
────────────────────────────────────────────────────────────────────────────────
*/
/**
 * @brief  generate a new unique ID for invoice
 * @return new invoice ID
 */
std::string generateInvoiceId() {
    size_t maxIdx = 0;
    if (!serviceInvoicesList.empty()) {
        auto lastInvoice = serviceInvoicesList.back();
        maxIdx           = std::stoull(lastInvoice.id.substr(2));
    }

    int    digitNumber = 0;
    size_t nextIdx     = maxIdx + 1;
    while (nextIdx > 0) {
        nextIdx /= 10;
        digitNumber++;
    }
    std::string newId = "SI";
    for (int i = 0; i < 8 - digitNumber; i++) {
        newId += "0";
    }
    newId += std::to_string(maxIdx + 1);

    return newId;
}

/*
────────────────────────────────────────────────────────────────────────────────
Calculate service fee
────────────────────────────────────────────────────────────────────────────────
*/
/**
 * @brief  calculate electricity fee
 * @param  oldIndex: old electricity index
 * @param  newIndex: new electricity index
 * @return electricity fee
 */
double electricityFee(double oldIndex, double newIndex) {
    double used = newIndex - oldIndex;
    if (used <= 0)
        return 0;

    double total = 0;
    if (used <= ELECTRICITY_TIER[0]) {
        total = used * ELECTRICITY_RATE[0];
    } else if (used <= ELECTRICITY_TIER[1]) {
        total = ELECTRICITY_TIER[0] * ELECTRICITY_RATE[0] +
                (used - ELECTRICITY_TIER[0]) * ELECTRICITY_RATE[1];
    } else if (used <= ELECTRICITY_TIER[2]) {
        total =
            ELECTRICITY_TIER[0] * ELECTRICITY_RATE[0] +
            (ELECTRICITY_TIER[1] - ELECTRICITY_TIER[0]) * ELECTRICITY_RATE[1] +
            (used - ELECTRICITY_TIER[1]) * ELECTRICITY_RATE[2];
    } else {
        total =
            ELECTRICITY_TIER[0] * ELECTRICITY_RATE[0] +
            (ELECTRICITY_TIER[1] - ELECTRICITY_TIER[0]) * ELECTRICITY_RATE[1] +
            (ELECTRICITY_TIER[2] - ELECTRICITY_TIER[1]) * ELECTRICITY_RATE[2] +
            (used - ELECTRICITY_TIER[2]) * ELECTRICITY_RATE[3];
    }
    return total;
}

/**
 * @brief  calculate water fee
 * @param  oldIndex: old water index
 * @param  newIndex: new water index
 * @return water fee
 */
double waterFee(double oldIndex, double newIndex) {
    if (newIndex < oldIndex)
        return 0;
    return (newIndex - oldIndex) * WATER_RATE;
}

/*
────────────────────────────────────────────────────────────────────────────────
Find service invoice
────────────────────────────────────────────────────────────────────────────────
*/
/**
 * @brief  find invoice by invoiceId
 * @param  invoiceId: id of invoice to find
 * @return lower bound index of invoice in serviceInvoicesList
 */
size_t findInvoice(const std::string& invoiceId) {
    auto it =
        binarySearch(serviceInvoicesList, invoiceId,
                     [](const ServiceInvoice& invoice, const std::string& id) {
                         return invoice.id < id;
                     });

    return it - serviceInvoicesList.begin();
}

/**
 * @brief  find invoice of room on (month, year)
 * @param  roomId   : ID of the room
 * @param  month    : month of the invoice
 * @param  year     : year of the invoice
 * @return index of latest invoice is found, serviceInvoicesList.size()
 * otherwise
 */
size_t findInvoice(const std::string& roomId, size_t month, size_t year) {
    for (auto& invoice : serviceInvoicesList) {
        if (invoice.roomId == roomId && invoice.month == month &&
            invoice.year == year) {
            return &invoice - serviceInvoicesList.begin();
        }
    }
    return serviceInvoicesList.size();
}

/**
 * @brief  find the latest invoice for a specific room
 * @param  roomId: ID of the room
 * @return index of the latest invoice if found, serviceInvoicesList.size()
 * otherwise
 */
size_t findLastInvoice(const std::string& roomId) {
    for (auto it = serviceInvoicesList.end(); it != serviceInvoicesList.begin();
         it--) {
        if ((it - 1)->roomId == roomId) {
            return (it - 1) - serviceInvoicesList.begin();
        }
    }
    return serviceInvoicesList.size();
}

/*
────────────────────────────────────────────────────────────────────────────────
Service invoice management
────────────────────────────────────────────────────────────────────────────────
 */

/**
 * @brief  create new invoice and add to serviceInvoicesList
 * @param  roomId              : ID of the room
 * @param  month               : month of the invoice
 * @param  year                : year of the invoice
 * @param  newElectricityIndex : new electricity index
 * @param  newWaterIndex       : new water index
 */
void createInvoice(const std::string& roomId, size_t month, size_t year,
                   double newElectricityIndex, double newWaterIndex) {
    if (findInvoice(roomId, month, year) != serviceInvoicesList.size()) {
        return;
    }

    ServiceInvoice newInvoice;

    newInvoice.id     = generateInvoiceId();
    newInvoice.roomId = roomId;
    newInvoice.month  = month;
    newInvoice.year   = year;

    auto lastIdx = findLastInvoice(roomId);
    if (lastIdx != serviceInvoicesList.size()) {
        auto& lastInvoice = serviceInvoicesList[lastIdx];

        newInvoice.oldElectricityIndex = lastInvoice.newElectricityIndex;
        newInvoice.oldWaterIndex       = lastInvoice.newWaterIndex;
    } else {
        newInvoice.oldElectricityIndex = 0;
        newInvoice.oldWaterIndex       = 0;
    }
    newInvoice.newElectricityIndex = newElectricityIndex;
    newInvoice.newWaterIndex       = newWaterIndex;
    newInvoice.totalAmount =
        electricityFee(newInvoice.oldElectricityIndex, newElectricityIndex) +
        waterFee(newInvoice.oldWaterIndex, newWaterIndex);

    newInvoice.isPaid = false;

    serviceInvoicesList.push_back(newInvoice);
}

/**
 * @brief  update payment status of an invoice
 * @param  invoiceId : id of the invoice to update
 * @param  status    : new payment status
 */
void updatePaymentStatus(const std::string& invoiceId, const bool& status) {
    size_t idx = findInvoice(invoiceId);
    if (idx == serviceInvoicesList.size() ||
        serviceInvoicesList[idx].id != invoiceId) {
        return;
    }
    serviceInvoicesList[idx].isPaid = status;
}