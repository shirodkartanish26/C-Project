#ifndef COMMON_H
#define COMMON_H

#include <iostream>
#include <string>
#include <limits>

// Console colors
#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define RESET   "\033[0m"

// Room status
enum class RoomStatus { Available, Booked, Maintenance };

// Convert status to text
inline std::string statusToString(RoomStatus s) {
    switch (s) {
        case RoomStatus::Available: return "Available";
        case RoomStatus::Booked: return "Booked";
        case RoomStatus::Maintenance: return "Maintenance";
    }
    return "Unknown";
}

#endif
