cmake_minimum_required(VERSION 3.16)

project(MSD_Desktop LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort)

qt_standard_project_setup()

qt_add_executable(MSD_Desktop
    main.cpp
)

target_link_libraries(MSD_Desktop
    Qt6::Widgets
    Qt6::SerialPort
)
