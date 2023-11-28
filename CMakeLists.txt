cmake_minimum_required(VERSION 3.20)

project(backgammon)

set(CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "src/*.cc")

add_executable(backgammon ${sources})

target_include_directories(backgammon PRIVATE "include")
target_link_libraries(backgammon PRIVATE ncursesw)
