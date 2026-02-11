#ifndef TIMESHEET_DB_H
#define TIMESHEET_DB_H

#include <sqlite3.h>
#include <string>
#include <vector>

struct TimesheetEntry {
    int id;
    std::string employee_name;
    std::string check_in_time;
    std::string check_out_time;
    std::string tasks;
    std::string date;
};

class TimesheetDB {
private:
    sqlite3* db;
    std::string db_path;

public:
    TimesheetDB(const std::string& db_file);
    ~TimesheetDB();
    
    bool initialize();
    bool checkIn(const std::string& employee_name);
    bool checkOut(const std::string& employee_name, const std::string& tasks);
    std::vector<TimesheetEntry> getEntries(const std::string& employee_name = "");
    std::vector<TimesheetEntry> getTodayEntries();
};

#endif // TIMESHEET_DB_H
