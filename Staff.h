#ifndef STAFF_H
#define STAFF_H

#include <string>

class Staff {
public:
    int staffId;
    std::string name;
    std::string role;
    double salary;

    Staff() = default;

    Staff(int id, const std::string &n, const std::string &r, double s)
        : staffId(id), name(n), role(r), salary(s) {}
};

#endif
