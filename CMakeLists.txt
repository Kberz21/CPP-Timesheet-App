cmake_minimum_required(VERSION 3.10)
project(TimesheetApp)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Find SQLite3
find_package(SQLite3 REQUIRED)
include_directories(${SQLite3_INCLUDE_DIRS})

# Add source files
set(SOURCES
    src/main.cpp
    src/timesheet_db.cpp
)

# Create executable
add_executable(timesheet ${SOURCES})

# Link SQLite3 library
target_link_libraries(timesheet ${SQLite3_LIBRARIES})
