#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <string>

class Customer {
public:
    int custId;
    std::string name;
    std::string phone;
    std::string email;

    Customer() = default;

    Customer(int id, const std::string &n, const std::string &ph, const std::string &em)
        : custId(id), name(n), phone(ph), email(em) {}
};

#endif
