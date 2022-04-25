cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# "1" will be reserved for the time, this is determined "stable"
# afterwards this number will increase on major overhauls, making it incompatible
# with e.g. linked libraries
set(APP_VERSION_MAJOR "0")
# next one will be changed, when a conifuguration incompatibility occurs
set(APP_VERSION_MINOR "0")
# this will increase every time something changes
set(APP_VERSION_BUILD "0")

# prepare infos for the details section of the executable
set(APP_VERSION "${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_BUILD}")
set(APP_PRODUCT "Plag'n")
set(APP_COPYRIGHT "LGPL v2.1")

project(plagn VERSION ${APP_VERSION} LANGUAGES CXX)

file(GLOB SOURCES "src/*.cpp"
                  "src/utils/*.cpp"
                  "src/datagrams/*.cpp")

add_executable(plagn ${SOURCES})

# header files added to include dirs
target_include_directories(plagn PRIVATE include
                                         include/datagrams
                                         include/utils)
# source files added to include dirs
target_include_directories(plagn PRIVATE src)
