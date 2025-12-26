cmake_minimum_required(VERSION 3.31)
project(wildcat2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Pull git information

execute_process(
        COMMAND git log -1 --format=%h
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        OUTPUT_VARIABLE GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(wildcat2 src/main.cpp src/driver/wrapper.cpp
        include/Wildcat/ui/mainwindow.h
        src/ui/mainwindow.cpp
        include/Wildcat/ui/channelswidget.h
        src/ui/channelswidget.cpp
        include/Wildcat/io/channel.h
        src/io/device.cpp
        include/Wildcat/io/device.h
        src/io/channel.cpp
        include/Wildcat/io/message.h
        src/io/message.cpp
        include/Wildcat/io/ctcss.h
        src/io/ctcss.cpp
        include/Wildcat/ui/connectionwidget.h
        src/ui/connectionwidget.cpp)

# Wildcat platform libraries (static)
# On linux we have our own user space "driver" that couples with the linux-specific serial IO "driver" used inside wildcat for communications
# Windows impl is TBD

if (WIN32)
    # TODO: Windows platform code
else()
    # Linux platform code
    add_library(wildcatplatform2 STATIC src/io/iodrivers/linux64.cpp)

    add_executable(wildcatdriver2 src/driver/environ.cpp src/driver/main.cpp src/driver/driver.cpp)
    target_include_directories(wildcatdriver2 PUBLIC include)
endif()

target_include_directories(wildcat2 PUBLIC include)
target_include_directories(wildcatplatform2 PUBLIC include)

target_compile_definitions(wildcat2 PUBLIC GIT_HASH_BUILD="${GIT_HASH}" GIT_BRANCH_BUILD="${GIT_BRANCH}")

target_link_libraries(wildcatplatform2 PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
)

target_link_libraries(wildcat2
        PUBLIC
        Qt::Core
        Qt::Gui
        Qt::Widgets
        wildcatplatform2
)