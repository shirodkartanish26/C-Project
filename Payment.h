#ifndef PAYMENT_H
#define PAYMENT_H

#include <string>
#include <sstream>
#include <iomanip>

class Payment {
public:
    int paymentId;
    int bookingId;
    double baseAmount;
    double extraCharges;
    double taxRate;

    Payment() = default;

    Payment(int pid, int bid, double base, double extra, double tax)
        : paymentId(pid), bookingId(bid),
          baseAmount(base), extraCharges(extra), taxRate(tax) {}

    double calculateTax() const {
        return (baseAmount + extraCharges) * taxRate;
    }

    double total() const {
        return baseAmount + extraCharges + calculateTax();
    }
};

#endif

