#ifndef MAINTENANCE_H
#define MAINTENANCE_H

#include <string>

enum class MaintStatus { Scheduled, InProgress, Completed };

class Maintenance {
public:
    int maintId;
    int roomId;
    std::string issue;
    MaintStatus status;
    std::string scheduledDate;

    Maintenance() = default;

    Maintenance(int id, int r, const std::string &iss, const std::string &date)
        : maintId(id), roomId(r), issue(iss),
          status(MaintStatus::Scheduled), scheduledDate(date) {}
};

#endif

