#include "timesheet_db.h"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>

TimesheetDB::TimesheetDB(const std::string& db_file) : db(nullptr), db_path(db_file) {}

TimesheetDB::~TimesheetDB() {
    if (db) {
        sqlite3_close(db);
    }
}

bool TimesheetDB::initialize() {
    int rc = sqlite3_open(db_path.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    const char* sql = 
        "CREATE TABLE IF NOT EXISTS timesheet ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "employee_name TEXT NOT NULL,"
        "check_in_time TEXT NOT NULL,"
        "check_out_time TEXT,"
        "tasks TEXT,"
        "date TEXT NOT NULL);";

    char* err_msg = nullptr;
    rc = sqlite3_exec(db, sql, nullptr, nullptr, &err_msg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
        return false;
    }

    return true;
}

std::string getCurrentDateTime() {
    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return std::string(buffer);
}

std::string getCurrentDate() {
    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", timeinfo);
    return std::string(buffer);
}

bool TimesheetDB::checkIn(const std::string& employee_name) {
    std::string current_time = getCurrentDateTime();
    std::string current_date = getCurrentDate();
    
    // Check if employee already checked in today
    std::string check_sql = "SELECT COUNT(*) FROM timesheet WHERE employee_name = ? AND date = ? AND check_out_time IS NULL;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, check_sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, employee_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, current_date.c_str(), -1, SQLITE_TRANSIENT);
    
    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    
    if (count > 0) {
        std::cerr << "Error: Employee " << employee_name << " is already checked in today." << std::endl;
        return false;
    }
    
    // Insert new check-in record
    const char* sql = "INSERT INTO timesheet (employee_name, check_in_time, date) VALUES (?, ?, ?);";
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, employee_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, current_time.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, current_date.c_str(), -1, SQLITE_TRANSIENT);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    return true;
}

bool TimesheetDB::checkOut(const std::string& employee_name, const std::string& tasks) {
    std::string current_time = getCurrentDateTime();
    std::string current_date = getCurrentDate();
    
    // Update the most recent check-in record without check-out for today
    const char* sql = "UPDATE timesheet SET check_out_time = ?, tasks = ? WHERE employee_name = ? AND date = ? AND check_out_time IS NULL;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    sqlite3_bind_text(stmt, 1, current_time.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, tasks.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, employee_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, current_date.c_str(), -1, SQLITE_TRANSIENT);
    
    int rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    if (sqlite3_changes(db) == 0) {
        std::cerr << "Error: No active check-in found for employee " << employee_name << " today." << std::endl;
        return false;
    }
    
    return true;
}

std::vector<TimesheetEntry> TimesheetDB::getEntries(const std::string& employee_name) {
    std::vector<TimesheetEntry> entries;
    std::string sql;
    
    if (employee_name.empty()) {
        sql = "SELECT id, employee_name, check_in_time, check_out_time, tasks, date FROM timesheet ORDER BY date DESC, check_in_time DESC;";
    } else {
        sql = "SELECT id, employee_name, check_in_time, check_out_time, tasks, date FROM timesheet WHERE employee_name = ? ORDER BY date DESC, check_in_time DESC;";
    }
    
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return entries;
    }
    
    if (!employee_name.empty()) {
        sqlite3_bind_text(stmt, 1, employee_name.c_str(), -1, SQLITE_TRANSIENT);
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        TimesheetEntry entry;
        entry.id = sqlite3_column_int(stmt, 0);
        
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        entry.employee_name = name ? std::string(name) : "";
        
        const char* check_in = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        entry.check_in_time = check_in ? std::string(check_in) : "";
        
        if (sqlite3_column_type(stmt, 3) != SQLITE_NULL) {
            const char* check_out = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            entry.check_out_time = check_out ? std::string(check_out) : "N/A";
        } else {
            entry.check_out_time = "N/A";
        }
        
        if (sqlite3_column_type(stmt, 4) != SQLITE_NULL) {
            const char* tasks = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            entry.tasks = tasks ? std::string(tasks) : "N/A";
        } else {
            entry.tasks = "N/A";
        }
        
        const char* date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        entry.date = date ? std::string(date) : "";
        
        entries.push_back(entry);
    }
    
    sqlite3_finalize(stmt);
    return entries;
}

std::vector<TimesheetEntry> TimesheetDB::getTodayEntries() {
    std::vector<TimesheetEntry> entries;
    std::string current_date = getCurrentDate();
    
    const char* sql = "SELECT id, employee_name, check_in_time, check_out_time, tasks, date FROM timesheet WHERE date = ? ORDER BY check_in_time DESC;";
    sqlite3_stmt* stmt;
    
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return entries;
    }
    
    sqlite3_bind_text(stmt, 1, current_date.c_str(), -1, SQLITE_TRANSIENT);
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        TimesheetEntry entry;
        entry.id = sqlite3_column_int(stmt, 0);
        
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        entry.employee_name = name ? std::string(name) : "";
        
        const char* check_in = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        entry.check_in_time = check_in ? std::string(check_in) : "";
        
        if (sqlite3_column_type(stmt, 3) != SQLITE_NULL) {
            const char* check_out = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            entry.check_out_time = check_out ? std::string(check_out) : "N/A";
        } else {
            entry.check_out_time = "N/A";
        }
        
        if (sqlite3_column_type(stmt, 4) != SQLITE_NULL) {
            const char* tasks = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            entry.tasks = tasks ? std::string(tasks) : "N/A";
        } else {
            entry.tasks = "N/A";
        }
        
        const char* date = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        entry.date = date ? std::string(date) : "";
        
        entries.push_back(entry);
    }
    
    sqlite3_finalize(stmt);
    return entries;
}
