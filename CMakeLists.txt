cmake_minimum_required(VERSION 3.31)

project(wildcat2)

# Install CPM (CMake Package Manager)

file(DOWNLOAD
     https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.42.0/CPM.cmake
     ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

# Install dependencies (excluded QT due to build time and complexity, instead just depend on the system version)

CPMAddPackage("gh:nlohmann/json@3.12.0")
CPMAddPackage("gh:gabime/spdlog@1.17.0")

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
        include/Wildcat/driver/wrapper.h
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
        src/ui/connectionwidget.cpp
        include/Wildcat/ui/devicepicker.h
        src/ui/devicepicker.cpp
        include/Wildcat/io/iothread.h
        src/io/iothread.cpp
        include/Wildcat/io/basicfuture.h
        src/io/basicfuture.cpp
        include/Wildcat/ui/textinput.h
        src/ui/textedit.cpp
        include/Wildcat/fs/archive.h
        include/Wildcat/ui/importwindow.h
        src/ui/importwindow.cpp
        src/fs/archive.cpp
        src/fs/pastedsheet.cpp
        src/fs/csv.cpp
        src/driver/utils.cpp
        include/Wildcat/global.h
        include/Wildcat/fs/delimiter.h
        src/fs/delimiter.cpp
        include/Wildcat/fs/pastedsheet.h
        include/Wildcat/fs/csv.h
        include/Wildcat/fs/json.h
        src/fs/json.cpp)

qt_add_resources(wildcat2 "resources"
    PREFIX "/resources"
    FILES wcat2.png credits.txt)

# Wildcat platform libraries (static)
# On Linux we have our own user space "driver" that couples with the linux-specific serial IO "driver" used inside wildcat for communications
# On Windows we have the windows-specific serial IO "driver" which uses Windows COM ports to communicate with the scanner

if (MINGW)
  add_library(wildcatplatform2 STATIC src/io/iodrivers/win32com.cpp
        src/io/iodrivers/win32com.cpp)
else()
  # Linux platform code
  add_library(wildcatplatform2 STATIC src/io/iodrivers/unixtty.cpp)

  add_executable(wildcatdriver2 src/driver/environ.cpp src/driver/main.cpp src/driver/driver.cpp src/driver/utils.cpp)
  target_include_directories(wildcatdriver2 PUBLIC include)
endif()

target_include_directories(wildcat2 PUBLIC include)
target_include_directories(wildcatplatform2 PUBLIC include)

target_compile_definitions(wildcat2 PUBLIC GIT_HASH_BUILD="${GIT_HASH}" GIT_BRANCH_BUILD="${GIT_BRANCH}")

target_link_libraries(wildcatplatform2 PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
    spdlog
)

target_link_libraries(wildcat2
        PUBLIC
        Qt::Core
        Qt::Gui
        Qt::Widgets
        wildcatplatform2
        nlohmann_json::nlohmann_json
        spdlog
)

qt_standard_project_setup()

install(TARGETS wildcat2
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

if(LINUX)
  install(TARGETS wildcatdriver2
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

qt_generate_deploy_app_script(
        TARGET wildcat2
        OUTPUT_SCRIPT deploy_script
        NO_UNSUPPORTED_PLATFORM_ERROR)

install(SCRIPT ${deploy_script})
