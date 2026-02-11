# CPP-Timesheet-App

A terminal-based employee timesheet application built with C++ and SQLite. This app allows employees to check in, check out, log their daily tasks, and view timesheet records.

## Features

- **Check In**: Record employee arrival time
- **Check Out**: Record employee departure time and tasks completed
- **View Today's Records**: Display all timesheet entries for the current day
- **View All Records**: Display all historical timesheet entries
- **View Records by Employee**: Filter timesheet records by employee name
- **SQLite Database**: Local database storage for persistent data

## Prerequisites

- C++ compiler with C++11 support (g++, clang++)
- CMake (version 3.10 or higher)
- SQLite3 library

## Building the Application

1. Clone the repository:
```bash
git clone https://github.com/Kberz21/CPP-Timesheet-App.git
cd CPP-Timesheet-App
```

2. Create a build directory:
```bash
mkdir build
cd build
```

3. Run CMake and build:
```bash
cmake ..
make
```

## Running the Application

After building, run the application from the build directory:
```bash
./timesheet
```

Or from the root directory:
```bash
./build/timesheet
```

## Usage

When you run the application, you'll see a menu with the following options:

1. **Check In**: Enter an employee name to record their check-in time
2. **Check Out**: Enter an employee name and tasks completed to record check-out
3. **View Today's Records**: See all entries for today
4. **View All Records**: See all historical entries
5. **View Records by Employee**: Filter by specific employee name
6. **Exit**: Close the application

### Example Workflow

1. Start the application
2. Select option 1 (Check In) and enter "John Doe"
3. Later, select option 2 (Check Out), enter "John Doe" and describe tasks completed
4. Select option 3 to view today's records
5. Select option 6 to exit

## Database

The application creates a `timesheet.db` file in the current directory to store all timesheet data. The database includes:

- Employee name
- Check-in timestamp
- Check-out timestamp
- Tasks completed
- Date

## License

This project is created as a student coding exercise.