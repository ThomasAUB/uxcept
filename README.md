![build status](https://github.com/ThomasAUB/uxcept/actions/workflows/build.yml/badge.svg)
[![License](https://img.shields.io/github/license/ThomasAUB/uxcept)](LICENSE)

# uXcept

Lighweight C++17 exception library for microcontrollers.

- single-header
- no heap allocation
- platform independent

## Example

```cpp
#include "uxcept.hpp"

int buffer[3];

void mayFail(uint8_t i, int val) {
    uxcept::check(i < 3, "invalid index");
    buffer[i] = val;
}

int main() {

    uxcept::tryCatch(
        [&] () {
            mayFail(0, 55);
            mayFail(1, 74);
            mayFail(8, 12);
        },
        [] (std::string_view inError) {
            std::cout << "error : " << inError << std::endl;
        }
    );

    return 0;
}
```

An error can also be raised using **uxcept::raise(error)**.

## Custom error type

Note that it's possible to change the error type that is defaulted to std::string_view. For that, you only need to create a file called "uxcept_error_type.hpp" and define the error type with an alias.

```cpp
// uxcept_error_type.hpp
namespace uxcept { using error_type = int; }
```
