# OperationalCalendarCPP

[![CI](https://github.com/Burevistvishezor/OperationalCalendarCPP/actions/workflows/ci.yml/badge.svg)](https://github.com/Burevistvishezor/OperationalCalendarCPP/actions)

A small, dependency-free C++20 library for **banking business-day logic**:
settlement dates, "T+N" calculations and end-of-day cut-off checks.
Built on `std::chrono` calendar types (no third-party date library needed).

## Features

- `is_operational_day(date)` – weekend / holiday check, with support for
  *transferred working weekends* (e.g. a working Saturday).
- `add_operational_days(date, n)` – move `n` business days forward or backward.
- `is_after_cutoff(time, cutoff = 16:00:00)` – strict, second-accurate cut-off check.
- Input validation (`std::invalid_argument` on invalid dates), `[[nodiscard]]` API,
  overflow-safe offset handling.
- Unit tests (Catch2) and CI on Linux and Windows.

## Example

```cpp
#include <iostream>
#include "bank/operational_calendar.hpp"

using namespace std::chrono;

int main() {
    bank::OperationalCalendar cal{
        /* holidays            */ {2024y / January / 1, 2024y / May / 3},
        /* extra working days  */ {2024y / May / 4}};

    // Settlement T+2 from Friday 1 March 2024
    auto settle = cal.add_operational_days(2024y / March / 1, 2);
    std::cout << settle << '\n';                       // 2024-03-05

    hh_mm_ss<seconds> t{16h + 5min};
    std::cout << std::boolalpha << bank::is_after_cutoff(t) << '\n';   // true
}
```

## Build & test

Requires a C++20 compiler with `<chrono>` calendar support
(GCC 11+, MSVC 19.29+) and CMake 3.20+.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
ctest --test-dir build --output-on-failure
```

Catch2 is downloaded automatically via CMake `FetchContent`.

## Project layout

```
include/bank/operational_calendar.hpp   public API
src/operational_calendar.cpp            implementation
tests/test_operational_calendar.cpp     Catch2 unit tests
.github/workflows/ci.yml                GitHub Actions (Linux + Windows)
```

## Notes and limitations

- Holidays and extra working days are supplied by the caller (from a config
  file or a database); the library does not ship a national holiday table.
- Time zones are out of scope: `is_after_cutoff` works on a local time of day
  that the caller has already converted.
- Written as a portfolio project to demonstrate modern C++ in a domain I know
  from my work on core banking software.

## License

MIT – see [LICENSE](LICENSE).
