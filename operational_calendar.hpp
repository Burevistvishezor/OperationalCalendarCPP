#pragma once

#include <chrono>
#include <set>
#include <vector>

namespace bank {

using date_type = std::chrono::year_month_day;

/// Business-day calendar for banking operations.
///
/// A day is *operational* if
///   1. it is listed in `extra_working_days` (e.g. a transferred working
///      Saturday), or
///   2. it is neither a Saturday/Sunday nor a listed holiday.
///
/// All dates passed to the public API must be valid (`date.ok()`),
/// otherwise `std::invalid_argument` is thrown.
class OperationalCalendar {
public:
    explicit OperationalCalendar(const std::vector<date_type>& holidays,
                                 const std::vector<date_type>& extra_working_days = {});

    /// True if the bank processes operations on the given date.
    [[nodiscard]] bool is_operational_day(date_type d) const;

    /// Moves `count` operational days forward (count > 0) or backward
    /// (count < 0). `count == 0` returns `start` unchanged, even if `start`
    /// itself is not an operational day.
    [[nodiscard]] date_type add_operational_days(date_type start, long long count) const;

private:
    std::set<std::chrono::sys_days> holidays_;
    std::set<std::chrono::sys_days> extra_working_days_;
};

/// True if `t` is strictly later than `cutoff` (default 16:00:00).
/// 16:00:00 is still on time, 16:00:01 is already after the cutoff.
[[nodiscard]] bool is_after_cutoff(
    const std::chrono::hh_mm_ss<std::chrono::seconds>& t,
    std::chrono::seconds cutoff = std::chrono::hours{16});

} // namespace bank
