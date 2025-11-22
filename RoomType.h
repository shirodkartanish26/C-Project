#ifndef ROOMTYPE_H
#define ROOMTYPE_H

#include <string>

class RoomType {
public:
    int typeId;
    std::string name;
    double basePrice;
    std::string features;

    RoomType() = default;

    RoomType(int id, const std::string &n, double p, const std::string &f)
        : typeId(id), name(n), basePrice(p), features(f) {}
};

#endif
