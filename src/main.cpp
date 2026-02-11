#include "timesheet_db.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <limits>

void displayMenu() {
    std::cout << "\n====================================\n";
    std::cout << "   EMPLOYEE TIMESHEET SYSTEM\n";
    std::cout << "====================================\n";
    std::cout << "1. Check In\n";
    std::cout << "2. Check Out\n";
    std::cout << "3. View Today's Records\n";
    std::cout << "4. View All Records\n";
    std::cout << "5. View Records by Employee\n";
    std::cout << "6. Exit\n";
    std::cout << "====================================\n";
    std::cout << "Enter your choice: ";
}

void displayEntries(const std::vector<TimesheetEntry>& entries) {
    if (entries.empty()) {
        std::cout << "\nNo records found.\n";
        return;
    }
    
    std::cout << "\n" << std::string(120, '=') << "\n";
    std::cout << std::left 
              << std::setw(5) << "ID" 
              << std::setw(20) << "Employee" 
              << std::setw(20) << "Check-In" 
              << std::setw(20) << "Check-Out" 
              << std::setw(15) << "Date"
              << std::setw(40) << "Tasks" 
              << "\n";
    std::cout << std::string(120, '=') << "\n";
    
    for (const auto& entry : entries) {
        std::cout << std::left 
                  << std::setw(5) << entry.id 
                  << std::setw(20) << entry.employee_name 
                  << std::setw(20) << entry.check_in_time 
                  << std::setw(20) << entry.check_out_time 
                  << std::setw(15) << entry.date
                  << std::setw(40) << entry.tasks 
                  << "\n";
    }
    std::cout << std::string(120, '=') << "\n";
}

void handleCheckIn(TimesheetDB& db) {
    std::string employee_name;
    std::cout << "\nEnter employee name: ";
    std::getline(std::cin, employee_name);
    
    if (employee_name.empty()) {
        std::cout << "Error: Employee name cannot be empty.\n";
        return;
    }
    
    if (db.checkIn(employee_name)) {
        std::cout << "\nSuccess! " << employee_name << " has been checked in.\n";
    } else {
        std::cout << "\nFailed to check in.\n";
    }
}

void handleCheckOut(TimesheetDB& db) {
    std::string employee_name;
    std::string tasks;
    
    std::cout << "\nEnter employee name: ";
    std::getline(std::cin, employee_name);
    
    if (employee_name.empty()) {
        std::cout << "Error: Employee name cannot be empty.\n";
        return;
    }
    
    std::cout << "Enter tasks completed (press Enter when done): ";
    std::getline(std::cin, tasks);
    
    if (db.checkOut(employee_name, tasks)) {
        std::cout << "\nSuccess! " << employee_name << " has been checked out.\n";
    } else {
        std::cout << "\nFailed to check out.\n";
    }
}

void handleViewToday(TimesheetDB& db) {
    std::cout << "\n=== Today's Timesheet Records ===\n";
    auto entries = db.getTodayEntries();
    displayEntries(entries);
}

void handleViewAll(TimesheetDB& db) {
    std::cout << "\n=== All Timesheet Records ===\n";
    auto entries = db.getEntries();
    displayEntries(entries);
}

void handleViewByEmployee(TimesheetDB& db) {
    std::string employee_name;
    std::cout << "\nEnter employee name: ";
    std::getline(std::cin, employee_name);
    
    if (employee_name.empty()) {
        std::cout << "Error: Employee name cannot be empty.\n";
        return;
    }
    
    std::cout << "\n=== Timesheet Records for " << employee_name << " ===\n";
    auto entries = db.getEntries(employee_name);
    displayEntries(entries);
}

int main() {
    TimesheetDB db("timesheet.db");
    
    if (!db.initialize()) {
        std::cerr << "Failed to initialize database. Exiting...\n";
        return 1;
    }
    
    std::cout << "Database initialized successfully!\n";
    
    int choice;
    bool running = true;
    
    while (running) {
        displayMenu();
        std::cin >> choice;
        
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "\nInvalid input. Please enter a number.\n";
            continue;
        }
        
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        
        switch (choice) {
            case 1:
                handleCheckIn(db);
                break;
            case 2:
                handleCheckOut(db);
                break;
            case 3:
                handleViewToday(db);
                break;
            case 4:
                handleViewAll(db);
                break;
            case 5:
                handleViewByEmployee(db);
                break;
            case 6:
                std::cout << "\nThank you for using the Timesheet System. Goodbye!\n";
                running = false;
                break;
            default:
                std::cout << "\nInvalid choice. Please try again.\n";
        }
    }
    
    return 0;
}
