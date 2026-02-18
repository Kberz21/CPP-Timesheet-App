#ifndef TIMESHEET_DB_H
#define TIMESHEET_DB_H

#include <sqlite3.h>
#include <string>
#include <vector>
using namespace std;

struct TimesheetEntry {
    int id;
    string employee_name;
    string check_in_time;
    string check_out_time;
    string tasks;
    string date;
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
