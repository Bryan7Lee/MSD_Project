cmake_minimum_required(VERSION 3.16)
project(final_app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort)

qt_standard_project_setup()

add_executable(final_app
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

target_link_libraries(final_app
    Qt6::Widgets
    Qt6::SerialPort
)
