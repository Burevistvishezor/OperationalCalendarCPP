#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <stdexcept>

#include "bank/operational_calendar.hpp"

using namespace std::chrono;
using namespace std::chrono_literals;
using bank::OperationalCalendar;

namespace {

// 2024: Mon 1 Jan (holiday), Fri 3 May (holiday), Sat 4 May (working Saturday)
const OperationalCalendar cal{
    {2024y / January / 1, 2024y / May / 3},
    {2024y / May / 4},
};

} // namespace

TEST_CASE("weekends are not operational days", "[calendar]") {
    CHECK_FALSE(cal.is_operational_day(2024y / March / 2));  // Saturday
    CHECK_FALSE(cal.is_operational_day(2024y / March / 3));  // Sunday
    CHECK(cal.is_operational_day(2024y / March / 4));        // Monday
}

TEST_CASE("listed holidays are not operational days", "[calendar]") {
    CHECK_FALSE(cal.is_operational_day(2024y / January / 1));
    CHECK_FALSE(cal.is_operational_day(2024y / May / 3));
}

TEST_CASE("extra working days override the weekend rule", "[calendar]") {
    CHECK(cal.is_operational_day(2024y / May / 4));          // Saturday, but working
}

TEST_CASE("add_operational_days skips weekends", "[calendar]") {
    // Fri 1 Mar 2024 + 1 -> Mon 4 Mar
    CHECK(cal.add_operational_days(2024y / March / 1, 1) == 2024y / March / 4);
    // Fri 1 Mar 2024 + 5 -> Fri 8 Mar
    CHECK(cal.add_operational_days(2024y / March / 1, 5) == 2024y / March / 8);
}

TEST_CASE("add_operational_days skips holidays", "[calendar]") {
    // Fri 29 Dec 2023 + 1 -> Tue 2 Jan 2024 (Mon 1 Jan is a holiday)
    CHECK(cal.add_operational_days(2023y / December / 29, 1) == 2024y / January / 2);
}

TEST_CASE("add_operational_days counts working Saturdays", "[calendar]") {
    // Thu 2 May + 1 -> Fri 3 May is a holiday -> Sat 4 May is working
    CHECK(cal.add_operational_days(2024y / May / 2, 1) == 2024y / May / 4);
}

TEST_CASE("add_operational_days works backwards", "[calendar]") {
    // Mon 4 Mar 2024 - 1 -> Fri 1 Mar
    CHECK(cal.add_operational_days(2024y / March / 4, -1) == 2024y / March / 1);
    // Tue 2 Jan 2024 - 1 -> Fri 29 Dec 2023
    CHECK(cal.add_operational_days(2024y / January / 2, -1) == 2023y / December / 29);
}

TEST_CASE("zero offset returns start unchanged", "[calendar]") {
    const year_month_day saturday = 2024y / March / 2;
    CHECK(cal.add_operational_days(saturday, 0) == saturday);
}

TEST_CASE("invalid dates are rejected", "[calendar]") {
    const year_month_day invalid = 2024y / February / 30;
    CHECK_THROWS_AS(cal.is_operational_day(invalid), std::invalid_argument);
    CHECK_THROWS_AS(cal.add_operational_days(invalid, 1), std::invalid_argument);
    CHECK_THROWS_AS(OperationalCalendar({invalid}), std::invalid_argument);
}

TEST_CASE("cutoff: strictly after 16:00:00", "[cutoff]") {
    using hms = hh_mm_ss<seconds>;
    CHECK_FALSE(bank::is_after_cutoff(hms{15h + 59min + 59s}));
    CHECK_FALSE(bank::is_after_cutoff(hms{16h}));
    CHECK(bank::is_after_cutoff(hms{16h + 1s}));   // seconds matter
    CHECK(bank::is_after_cutoff(hms{16h + 1min}));
    CHECK(bank::is_after_cutoff(hms{23h}));
}

TEST_CASE("cutoff can be customised", "[cutoff]") {
    using hms = hh_mm_ss<seconds>;
    CHECK(bank::is_after_cutoff(hms{15h + 30min}, 15h));
    CHECK_FALSE(bank::is_after_cutoff(hms{15h}, 15h));
}
