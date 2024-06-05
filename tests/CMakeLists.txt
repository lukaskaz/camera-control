cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)
project(camera-control-ut)

enable_testing()

include(cmake/dependencies.cmake)
include(cmake/flags.cmake)

include_directories(inc)
file(GLOB SOURCES "src/*.cpp")

add_executable(${PROJECT_NAME} ${SOURCES})
add_dependencies(${PROJECT_NAME} googletest)
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
    Threads::Threads
    gtest
    gmock
)
