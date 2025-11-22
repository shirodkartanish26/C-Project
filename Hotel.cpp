#include "Hotel.h"
#include <sstream>
#include <iomanip>

/* ================= Constructor & Init ================= */

Hotel::Hotel() {
    // Load saved data
    loadAll();

    // First run: no rooms & no room types
    if (rooms.empty() && roomTypes.empty()) {
        initDefaultData();
        saveAll();
        return;
    }

    // Older program versions never saved roomTypes,
    // so we must regenerate them if missing.
    if (roomTypes.empty()) {
        initDefaultData();   // This only adds room types due to updated function
        saveAll();
    }
}

/* ================= Default Data Setup ================= */

void Hotel::initDefaultData() {
    // Create Room Types only if none exist
    if (roomTypes.empty()) {
        addRoomType(RoomType(1, "Standard", 1500.0, "Basic amenities"));
        addRoomType(RoomType(2, "Premium", 3000.0, "Sea view, AC"));
        addRoomType(RoomType(3, "Most Premium", 5000.0, "Suite, sea view, extras"));
    }

    // Create default rooms only if none exist
    if (rooms.empty()) {
        addRoom(1); addRoom(1); addRoom(1);
        addRoom(2); addRoom(2); addRoom(2);
        addRoom(3); addRoom(3); addRoom(3);
    }

    // Add default staff only once
    if (staff.empty()) {
        staff.push_back(Staff(1, "Admin User", "Administrator", 30000));
        staff.push_back(Staff(2, "Reception", "Staff", 15000));
    }
}

/* ================= Room / Customer Functions ================= */

void Hotel::addRoomType(const RoomType& rt) {
    roomTypes.push_back(rt);
}

const RoomType* Hotel::getRoomTypeById(int id) const {
    for (const auto& t : roomTypes)
        if (t.typeId == id) return &t;
    return nullptr;
}

void Hotel::addRoom(int typeId, double customPrice) {
    rooms.push_back(Room(nextRoomId++, typeId, customPrice));
}

Room* Hotel::findRoom(int roomId) {
    for (auto& r : rooms)
        if (r.roomId == roomId) return &r;
    return nullptr;
}

bool Hotel::hasRoom(int roomId) const {
    for (const auto& r : rooms)
        if (r.roomId == roomId) return true;
    return false;
}

bool Hotel::hasCustomer(int custId) const {
    for (const auto& c : customers)
        if (c.custId == custId) return true;
    return false;
}

bool Hotel::hasBooking(int bookingId) const {
    for (const auto& b : bookings)
        if (b.bookingId == bookingId) return true;
    return false;
}

int Hotel::addCustomer(const std::string& name, const std::string& phone, const std::string& email) {
    customers.emplace_back(nextCustomerId, name, phone, email);
    int id = nextCustomerId++;
    saveAll();
    return id;
}

int Hotel::createBooking(int custId, int roomId, int days) {
    Room* r = findRoom(roomId);
    if (!r) return -1;
    if (r->status != RoomStatus::Available) return -2;

    r->status = RoomStatus::Booked;
    bookings.emplace_back(nextBookingId, custId, roomId, r->typeId, days);
    int bid = nextBookingId++;
    saveAll();
    return bid;
}

bool Hotel::cancelBooking(int bookingId) {
    Booking* b = findBooking(bookingId);
    if (!b) return false;
    b->status = BookingStatus::Cancelled;

    Room* r = findRoom(b->roomId);
    if (r) r->status = RoomStatus::Available;

    saveAll();
    return true;
}

Booking* Hotel::findBooking(int bookingId) {
    for (auto& b : bookings)
        if (b.bookingId == bookingId) return &b;
    return nullptr;
}

/* ================= Checkout ================= */

void Hotel::checkoutBooking(int bookingId, double extraCharges) {
    Booking* b = findBooking(bookingId);
    if (!b) {
        std::cerr << "No booking found with ID " << bookingId << "\n";
        return;
    }

    Room* r = findRoom(b->roomId);
    if (!r) {
        std::cerr << "Room not found for booking.\n";
        return;
    }

    const RoomType* rt = getRoomTypeById(b->roomTypeId);
    if (!rt) {
        std::cerr << "Room type not found for booking.\n";
        return;
    }

    double base = rt->basePrice * b->days;

    payments.emplace_back(nextPaymentId, bookingId, base, extraCharges, TAX_RATE);
    nextPaymentId++;

    b->status = BookingStatus::CheckedOut;
    r->status = RoomStatus::Available;

    saveAll();

    // Generate invoice file
    std::stringstream ss;
    ss << "invoice_booking_" << bookingId << ".txt";
    std::ofstream f(ss.str());

    if (f) {
        f << "Hotel Blue Whale - Invoice\n";
        f << "Booking ID: " << bookingId << "\n";
        f << "Customer ID: " << b->custId << "\n";
        f << "Room ID: " << b->roomId << "\n";
        f << "Days: " << b->days << "\n";
        f << "Base: " << base << "\n";
        f << "Extra: " << extraCharges << "\n";
        f << "GST: " << payments.back().calculateTax() << "\n";
        f << "Total: " << payments.back().total() << "\n";
    }
}

/* ================= Maintenance ================= */

void Hotel::scheduleMaintenance(int roomId, const std::string& issue, const std::string& date) {
    Room* r = findRoom(roomId);
    if (!r) return;

    if (r->status == RoomStatus::Booked) {
        std::cout << "Cannot schedule maintenance. Room is booked.\n";
        return;
    }

    r->status = RoomStatus::Maintenance;
    maintenanceLogs.emplace_back(nextMaintId++, roomId, issue, date);
    saveAll();
}

void Hotel::toggleRoomMaintenance(int roomId) {
    Room* r = findRoom(roomId);
    if (!r) return;

    if (r->status == RoomStatus::Maintenance) {
        r->status = RoomStatus::Available;
    } else if (r->status == RoomStatus::Available) {
        r->status = RoomStatus::Maintenance;
    } else {
        std::cout << "Cannot change maintenance status. Room is currently booked.\n";
    }

    saveAll();
}

/* ================= Reporting Counts ================= */

int Hotel::countAvailableRooms() const {
    return std::count_if(rooms.begin(), rooms.end(),
                         [](const Room& r){ return r.status == RoomStatus::Available; });
}

int Hotel::countBookedRooms() const {
    return std::count_if(rooms.begin(), rooms.end(),
                         [](const Room& r){ return r.status == RoomStatus::Booked; });
}

int Hotel::countMaintenanceRooms() const {
    return std::count_if(rooms.begin(), rooms.end(),
                         [](const Room& r){ return r.status == RoomStatus::Maintenance; });
}

double Hotel::estimatedRevenueActive() const {
    double total = 0;

    for (const auto& b : bookings) {
        if (b.status == BookingStatus::Confirmed) {
            const RoomType* rt = getRoomTypeById(b.roomTypeId);
            if (rt) total += rt->basePrice * b.days;
        }
    }

    return total;
}

/* ================= Persistence ================= */

void Hotel::saveRooms() {
    std::ofstream f("rooms.dat");
    for (const auto& r : rooms) {
        f << r.roomId << " " << r.typeId << " " << (int)r.status << " " << r.customPrice << "\n";
    }
}

void Hotel::saveCustomers() {
    std::ofstream f("customers.dat");
    for (const auto& c : customers) {
        f << c.custId << " " << c.name << " " << c.phone << " " << c.email << "\n";
    }
}

void Hotel::saveBookings() {
    std::ofstream f("bookings.dat");
    for (const auto& b : bookings) {
        f << b.bookingId << " " << b.custId << " " << b.roomId << " "
          << b.roomTypeId << " " << b.days << " " << (int)b.status << "\n";
    }
}

void Hotel::savePayments() {
    std::ofstream f("payments.dat");
    for (const auto& p : payments) {
        f << p.paymentId << " " << p.bookingId << " " << p.baseAmount << " "
          << p.extraCharges << " " << p.taxRate << "\n";
    }
}

void Hotel::saveMaintenance() {
    std::ofstream f("maintenance.dat");
    for (const auto& m : maintenanceLogs) {
        f << m.maintId << " " << m.roomId << " " << m.issue << " "
          << (int)m.status << " " << m.scheduledDate << "\n";
    }
}

void Hotel::saveAll() {
    saveRooms();
    saveCustomers();
    saveBookings();
    savePayments();
    saveMaintenance();
}

/* ================= Load from Files ================= */

void Hotel::loadRooms() {
    std::ifstream f("rooms.dat");
    if (!f) return;

    rooms.clear();
    int id, type, status;
    double price;

    while (f >> id >> type >> status >> price) {
        Room r(id, type, price);
        r.status = (RoomStatus)status;
        rooms.push_back(r);
    }

    if (!rooms.empty()) {
        nextRoomId = rooms.back().roomId + 1;
    }
}

void Hotel::loadCustomers() {
    std::ifstream f("customers.dat");
    if (!f) return;

    customers.clear();
    int id;
    std::string name, phone, email;

    while (f >> id >> name >> phone >> email) {
        customers.emplace_back(id, name, phone, email);
    }

    if (!customers.empty()) {
        nextCustomerId = customers.back().custId + 1;
    }
}

void Hotel::loadBookings() {
    std::ifstream f("bookings.dat");
    if (!f) return;

    bookings.clear();
    int bid, cid, rid, rtid, days, status;

    while (f >> bid >> cid >> rid >> rtid >> days >> status) {
        Booking b(bid, cid, rid, rtid, days);
        b.status = (BookingStatus)status;
        bookings.push_back(b);
    }

    if (!bookings.empty()) {
        nextBookingId = bookings.back().bookingId + 1;
    }
}

void Hotel::loadPayments() {
    std::ifstream f("payments.dat");
    if (!f) return;

    payments.clear();
    int pid, bid;
    double base, extra, tax;

    while (f >> pid >> bid >> base >> extra >> tax) {
        payments.emplace_back(pid, bid, base, extra, tax);
    }

    if (!payments.empty()) {
        nextPaymentId = payments.back().paymentId + 1;
    }
}

void Hotel::loadMaintenance() {
    std::ifstream f("maintenance.dat");
    if (!f) return;

    maintenanceLogs.clear();
    int mid, rid, status;
    std::string issue, date;

    while (f >> mid >> rid >> issue >> status >> date) {
        Maintenance m(mid, rid, issue, date);
        m.status = (MaintStatus)status;
        maintenanceLogs.push_back(m);
    }

    if (!maintenanceLogs.empty()) {
        nextMaintId = maintenanceLogs.back().maintId + 1;
    }
}

void Hotel::loadAll() {
    loadRooms();
    loadCustomers();
    loadBookings();
    loadPayments();
    loadMaintenance();
}

/* ================= Utility Print Functions ================= */

void Hotel::printRooms() const {
    std::cout << "\n--- Rooms ---\n";
    for (const auto& r : rooms) {
        const RoomType* rt = getRoomTypeById(r.typeId);

        std::cout << "Room ID: " << r.roomId
                  << ", Type: " << (rt ? rt->name : "Unknown")
                  << ", Status: " << statusToString(r.status) << "\n";
    }
}

void Hotel::printCustomers() const {
    std::cout << "\n--- Customers ---\n";
    for (const auto& c : customers) {
        std::cout << "ID: " << c.custId
                  << ", Name: " << c.name
                  << ", Phone: " << c.phone
                  << ", Email: " << c.email << "\n";
    }
}

void Hotel::printBookings() const {
    std::cout << "\n--- Bookings ---\n";
    for (const auto& b : bookings) {
        std::cout << "Booking ID: " << b.bookingId
                  << ", Customer ID: " << b.custId
                  << ", Room ID: " << b.roomId
                  << ", Type ID: " << b.roomTypeId
                  << ", Days: " << b.days
                  << ", Status: " << (int)b.status << "\n";
    }
}

/* ================= Dashboard Export ================= */

void Hotel::exportToJSON() {
   std::ofstream f("dashboard/dashboard_data.json");

    
    f << "{\n";
    
    // Room statistics
    f << "  \"rooms\": {\n";
    f << "    \"total\": " << rooms.size() << ",\n";
    f << "    \"available\": " << countAvailableRooms() << ",\n";
    f << "    \"booked\": " << countBookedRooms() << ",\n";
    f << "    \"maintenance\": " << countMaintenanceRooms() << "\n";
    f << "  },\n";

    // Revenue summary
    double totalRevenue = 0;
    for (const auto& p : payments)
        totalRevenue += p.total();

    f << "  \"revenue\": {\n";
    f << "    \"completed\": " << totalRevenue << ",\n";
    f << "    \"estimated\": " << estimatedRevenueActive() << "\n";
    f << "  },\n";

    // Customers
    f << "  \"customers\": {\n";
    f << "    \"total\": " << customers.size() << "\n";
    f << "  },\n";

    // Booking status
    int confirmed = 0, cancelled = 0, checked = 0;
    for (const auto& b : bookings) {
        if (b.status == BookingStatus::Confirmed) confirmed++;
        else if (b.status == BookingStatus::Cancelled) cancelled++;
        else if (b.status == BookingStatus::CheckedOut) checked++;
    }

    f << "  \"bookings\": {\n";
    f << "    \"total\": " << bookings.size() << ",\n";
    f << "    \"confirmed\": " << confirmed << ",\n";
    f << "    \"cancelled\": " << cancelled << ",\n";
    f << "    \"checkedOut\": " << checked << "\n";
    f << "  },\n";

    // Room type revenue
    f << "  \"roomTypeRevenue\": [\n";
    for (size_t i = 0; i < roomTypes.size(); i++) {
        double typeRevenue = 0;
        for (const auto& p : payments) {
            const Booking* b = findBooking(p.bookingId);
            if (b && b->roomTypeId == roomTypes[i].typeId)
                typeRevenue += p.total();
        }

        f << "    {\"type\": \"" << roomTypes[i].name << "\", \"revenue\": " << typeRevenue << "}";
        if (i < roomTypes.size() - 1) f << ",";
        f << "\n";
    }
    f << "  ]\n";

    f << "}\n";
}

void Hotel::generateDashboardHTML() {
    exportToJSON();
    std::cout << "Dashboard data exported to dashboard_data.json\n";
}

/* ================= Dashboard (ASCII) ================= */

void Hotel::showDashboard() {
    std::cout << "\n===== HOTEL DASHBOARD =====\n";
    std::cout << "Total Rooms: " << rooms.size() << "\n";
    std::cout << "Available:   " << countAvailableRooms() << "\n";
    std::cout << "Booked:      " << countBookedRooms() << "\n";
    std::cout << "Maintenance: " << countMaintenanceRooms() << "\n";

    double totalRevenue = 0;
    for (const auto& p : payments)
        totalRevenue += p.total();

    std::cout << "Completed Revenue: ₹" << totalRevenue << "\n";
    std::cout << "Estimated Active Revenue: ₹" << estimatedRevenueActive() << "\n";
}


/* ================= Reports ================= */

void Hotel::printRoomsReport() {
    std::cout << "\n===== ROOMS REPORT =====\n";
    for (const auto& r : rooms) {
        const RoomType* rt = getRoomTypeById(r.typeId);
        std::cout << "Room " << r.roomId
                  << " | Type: " << (rt ? rt->name : "Unknown")
                  << " | Status: " << statusToString(r.status) << "\n";
    }
}

void Hotel::printCustomerReport() {
    std::cout << "\n===== CUSTOMER REPORT =====\n";
    for (const auto& c : customers) {
        std::cout << "ID: " << c.custId << " | "
                  << c.name << " | " << c.phone
                  << " | " << c.email << "\n";
    }
}

void Hotel::printRevenueReport() {
    std::cout << "\n===== REVENUE REPORT =====\n";

    double total = 0;
    for (const auto& p : payments)
        total += p.total();

    std::cout << "Completed Revenue: ₹" << total << "\n";
    std::cout << "Estimated Active Revenue: ₹" << estimatedRevenueActive() << "\n";
}

