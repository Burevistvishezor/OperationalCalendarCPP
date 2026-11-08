#include "bank/operational_calendar.hpp"

#include <stdexcept>

namespace bank {

namespace {

std::chrono::sys_days to_sys_days(date_type d) {
    if (!d.ok()) {
        throw std::invalid_argument("OperationalCalendar: invalid date");
    }
    return std::chrono::sys_days{d};
}

} // namespace

OperationalCalendar::OperationalCalendar(const std::vector<date_type>& holidays,
                                         const std::vector<date_type>& extra_working_days) {
    for (const auto& h : holidays) holidays_.insert(to_sys_days(h));
    for (const auto& w : extra_working_days) extra_working_days_.insert(to_sys_days(w));
}

bool OperationalCalendar::is_operational_day(date_type d) const {
    const auto sd = to_sys_days(d);

    if (extra_working_days_.contains(sd)) return true;

    const std::chrono::weekday wd{sd};
    if (wd == std::chrono::Saturday || wd == std::chrono::Sunday) return false;

    return !holidays_.contains(sd);
}

date_type OperationalCalendar::add_operational_days(date_type start, long long count) const {
    auto current = to_sys_days(start);
    if (count == 0) return start;

    const std::chrono::days step{count > 0 ? 1 : -1};
    // magnitude computed in unsigned arithmetic: safe even for LLONG_MIN
    unsigned long long remaining =
        count > 0 ? static_cast<unsigned long long>(count)
                  : 0ULL - static_cast<unsigned long long>(count);

    while (remaining > 0) {
        current += step;
        if (is_operational_day(date_type{current})) --remaining;
    }
    return date_type{current};
}

bool is_after_cutoff(const std::chrono::hh_mm_ss<std::chrono::seconds>& t,
                     std::chrono::seconds cutoff) {
    return t.to_duration() > cutoff;
}

} // namespace bank
