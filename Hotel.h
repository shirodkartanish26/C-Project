#ifndef HOTEL_H
#define HOTEL_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>

#include "Room.h"
#include "RoomType.h"
#include "Customer.h"
#include "Booking.h"
#include "Payment.h"
#include "Maintenance.h"
#include "Staff.h"

class Hotel {
private:
    std::vector<RoomType> roomTypes;
    std::vector<Room> rooms;
    std::vector<Customer> customers;
    std::vector<Booking> bookings;
    std::vector<Payment> payments;
    std::vector<Maintenance> maintenanceLogs;
    std::vector<Staff> staff;

    int nextRoomId  = 1;
    int nextCustomerId = 1;
    int nextBookingId  = 1;
    int nextPaymentId  = 1;
    int nextMaintId    = 1;

    const double TAX_RATE = 0.18;

    // Persistence helpers
    void saveRooms();
    void saveCustomers();
    void saveBookings();
    void savePayments();
    void saveMaintenance();
    
    void loadRooms();
    void loadCustomers();
    void loadBookings();
    void loadPayments();
    void loadMaintenance();

public:
    Hotel();
    
    // Add to public section of Hotel class
    void exportToJSON();
    void generateDashboardHTML();

    // Initialization helpers
    void initDefaultData();

    // RoomType & Room management
    void addRoomType(const RoomType& rt);
    const RoomType* getRoomTypeById(int id) const;
    void addRoom(int typeId, double customPrice = 0.0);
    Room* findRoom(int roomId);
    const std::vector<Room>& getRooms() const { return rooms; }

    // Customer & booking
    int addCustomer(const std::string& name, const std::string& phone, const std::string& email);
    int createBooking(int custId, int roomId, int days);
    bool cancelBooking(int bookingId);
    Booking* findBooking(int bookingId);

    const std::vector<Customer>& getCustomers() const { return customers; }
    const std::vector<Booking>& getBookings() const { return bookings; }

    bool hasRoom(int roomId) const;
    bool hasCustomer(int custId) const;
    bool hasBooking(int bookingId) const;

    void checkoutBooking(int bookingId, double extraCharges);

    // Maintenance
    void scheduleMaintenance(int roomId, const std::string& issue, const std::string& date);
    void toggleRoomMaintenance(int roomId);

    // Reports
    int countAvailableRooms() const;
    int countBookedRooms() const;
    int countMaintenanceRooms() const;
    double estimatedRevenueActive() const;

    // Persistence
    void saveAll();
    void loadAll();

    // Utilities
    void printRooms() const;
    void printCustomers() const;
    void printBookings() const;

    // Dashboard & reports
    void showAsciiGraph(const std::string& label, int value, int total);
    void showDashboard();
    void printRoomsReport();
    void printCustomerReport();
    void printRevenueReport();
};

#endif
