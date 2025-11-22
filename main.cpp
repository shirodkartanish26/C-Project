#include <iostream>
#include <string>
#include <limits>
#include <regex>

#include "Hotel.h"

// ---------- Safe Input Helpers ----------

int readInt(const std::string &prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            return value;
        } else {
            std::cout << "Invalid input. Please enter a number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

int readIntMin(const std::string &prompt, int minVal) {
    int value;
    while (true) {
        value = readInt(prompt);
        if (value >= minVal) return value;
        std::cout << "Value must be at least " << minVal << ". Try again.\n";
    }
}

double readDoubleMin(const std::string &prompt, double minVal) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            if (value >= minVal) return value;
            std::cout << "Value must be at least " << minVal << ". Try again.\n";
        } else {
            std::cout << "Invalid input. Please enter a number.\n";
        }
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::string readNonEmptyLine(const std::string &prompt) {
    std::string s;
    while (true) {
        std::cout << prompt;
        std::getline(std::cin, s);
        if (!s.empty()) return s;

        std::cout << "Value cannot be empty. Try again.\n";
    }
}

std::string readDate(const std::string &prompt) {
    std::regex pattern(R"(^\d{4}-\d{2}-\d{2}$)");
    std::string d;

    while (true) {
        std::cout << prompt;
        std::getline(std::cin, d);
        if (std::regex_match(d, pattern)) {
            return d;
        }
        std::cout << "Invalid date format. Use YYYY-MM-DD.\n";
    }
}

// ---------- Validation helpers for hotel IDs ----------

int readExistingRoomId(Hotel &hotel) {
    while (true) {
        int roomId;
        std::cout << "Room ID: ";
        if (!(std::cin >> roomId)) {
            std::cout << "Invalid number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (hotel.hasRoom(roomId)) return roomId;

        std::cout << "No such room. Try again.\n";
    }
}

int readExistingCustomerId(Hotel &hotel) {
    while (true) {
        int custId;
        std::cout << "Customer ID: ";
        if (!(std::cin >> custId)) {
            std::cout << "Invalid number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (hotel.hasCustomer(custId)) return custId;

        std::cout << "No such customer. Try again.\n";
    }
}

int readExistingBookingId(Hotel &hotel) {
    while (true) {
        int bid;
        std::cout << "Booking ID: ";
        if (!(std::cin >> bid)) {
            std::cout << "Invalid number.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        if (hotel.hasBooking(bid)) return bid;

        std::cout << "No such booking. Try again.\n";
    }
}

// ---------- Menu ----------

void showMenu() {
    std::cout << "\n=== Hotel Management System ===\n"
              << "1. Show Rooms\n"
              << "2. Show Dashboard\n"
              << "3. Add Customer\n"
              << "4. Create Booking\n"
              << "5. Cancel Booking\n"
              << "6. Checkout Booking (generate invoice)\n"
              << "7. Schedule Maintenance\n"
              << "8. Toggle Maintenance\n"
              << "9. Export / Save\n"
              << "10. Rooms Report\n"
              << "11. Customer Report\n"
              << "12. Revenue Report\n"
              << "13. Show All Bookings\n"
              << "14. Generate Web Dashboard\n"  // NEW OPTION
              << "0. Exit\n"
              << "Choose: ";
}

int main() {
    Hotel hotel;
    int choice;

    while (true) {
        showMenu();

        if (!(std::cin >> choice)) {
            std::cout << "Invalid menu option.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Flush

        if (choice == 0) break;

        if (choice == 1) {
            hotel.printRooms();

        } else if (choice == 2) {
            hotel.showDashboard();

        } else if (choice == 3) {
            std::string name  = readNonEmptyLine("Customer name: ");
            std::string phone = readNonEmptyLine("Phone: ");
            std::string email = readNonEmptyLine("Email: ");

            int id = hotel.addCustomer(name, phone, email);
            std::cout << "Customer added with ID: " << id << "\n";

        } else if (choice == 4) {
            int custId = readExistingCustomerId(hotel);
            int roomId = readExistingRoomId(hotel);
            int days   = readIntMin("Days: ", 1);

            int bid = hotel.createBooking(custId, roomId, days);

            if (bid > 0)
                std::cout << "Booking created with ID: " << bid << "\n";
            else if (bid == -2)
                std::cout << "Room is not available.\n";
            else
                std::cout << "Error creating booking.\n";

        } else if (choice == 5) {
            int bid = readExistingBookingId(hotel);
            hotel.cancelBooking(bid);

        } else if (choice == 6) {
            int bid    = readExistingBookingId(hotel);
            double ext = readDoubleMin("Extra charges: ", 0);
            hotel.checkoutBooking(bid, ext);

        } else if (choice == 7) {
            int roomId = readExistingRoomId(hotel);
            std::string issue = readNonEmptyLine("Issue: ");
            std::string date  = readDate("Scheduled date (YYYY-MM-DD): ");
            hotel.scheduleMaintenance(roomId, issue, date);

        } else if (choice == 8) {
            int roomId = readExistingRoomId(hotel);
            hotel.toggleRoomMaintenance(roomId);

        } else if (choice == 9) {
            hotel.saveAll();
            std::cout << "Data saved.\n";

        } else if (choice == 10) {
            hotel.printRoomsReport();

        } else if (choice == 11) {
            hotel.printCustomerReport();

        } else if (choice == 12) {
            hotel.printRevenueReport();

        } else if (choice == 13) {
            hotel.printBookings();

        } else if (choice == 14) {
            hotel.generateDashboardHTML();
            
        } else {
            std::cout << "Invalid menu option.\n";
        }
    }

    hotel.saveAll();
    std::cout << "Goodbye!\n";
    return 0;
}
