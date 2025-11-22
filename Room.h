#ifndef ROOM_H
#define ROOM_H

#include <string>
#include "COMMON.h"

class Room {
public:
    int roomId;
    int typeId;
    RoomStatus status;
    double customPrice;

    Room() = default;

    Room(int rid, int tid, double price)
        : roomId(rid), typeId(tid), status(RoomStatus::Available), customPrice(price) {}

    double effectivePrice(double basePrice) const {
        return (customPrice > 0.0) ? customPrice : basePrice;
    }
};

#endif

