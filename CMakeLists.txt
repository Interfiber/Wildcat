cmake_minimum_required(VERSION 3.5)

project(Wildcat LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("vendor/imgui")

# 1. Look for a SDL2 package, 2. look for the SDL2 component and 3. fail if none can be found
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)

# 1. Look for a SDL2 package, 2. Look for the SDL2maincomponent and 3. DO NOT fail when SDL2main is not available
find_package(SDL2 REQUIRED CONFIG COMPONENTS SDL2main)


include_directories(${SDL2_INCLUDE_DIRS})

add_executable(wildcat main.cpp vendor/imgui/backends/imgui_impl_sdl2.cpp vendor/imgui/backends/imgui_impl_sdlrenderer2.cpp  vendor/imgui/imgui.cpp vendor/imgui/imgui_draw.cpp vendor/imgui/imgui_widgets.cpp vendor/imgui/imgui_demo.cpp vendor/imgui/imgui_tables.cpp vendor/imgui/misc/cpp/imgui_stdlib.cpp
    macros.h
    wildcatmainwindow.h wildcatmainwindow.cpp
    wildcatchannel.h wildcatchannel.cpp
    wildcatuserdriver.h wildcatuserdriver.cpp
    wildcatauth.h wildcatauth.cpp)

add_executable(wildcatdriver main_driver.cpp wildcatuserdriver.h wildcatuserdriver.cpp)

target_link_libraries(wildcat SDL2::SDL2)
target_link_libraries(wildcatdriver SDL2::SDL2)

include(GNUInstallDirs)
install(TARGETS wildcat wildcatdriver
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
