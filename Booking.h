#ifndef BOOKING_H
#define BOOKING_H

#include <string>

enum class BookingStatus { Confirmed, Cancelled, CheckedOut };

class Booking {
public:
    int bookingId;
    int custId;
    int roomId;
    int roomTypeId;
    int days;
    BookingStatus status;

    Booking() = default;

    Booking(int b, int c, int r, int rt, int d)
        : bookingId(b), custId(c), roomId(r), roomTypeId(rt),
          days(d), status(BookingStatus::Confirmed) {}
};

#endif

