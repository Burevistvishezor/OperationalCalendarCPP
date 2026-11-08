cmake_minimum_required(VERSION 3.20)
project(OperationalCalendarCPP VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(operational_calendar src/operational_calendar.cpp)
target_include_directories(operational_calendar PUBLIC include)
if(MSVC)
  target_compile_options(operational_calendar PRIVATE /W4)
else()
  target_compile_options(operational_calendar PRIVATE -Wall -Wextra -Wpedantic)
endif()

option(BUILD_TESTING "Build unit tests" ON)
if(BUILD_TESTING)
  enable_testing()
  include(FetchContent)
  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.5.2)
  FetchContent_MakeAvailable(Catch2)

  add_executable(tests tests/test_operational_calendar.cpp)
  target_link_libraries(tests PRIVATE operational_calendar Catch2::Catch2WithMain)

  list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
  include(Catch)
  catch_discover_tests(tests)
endif()
