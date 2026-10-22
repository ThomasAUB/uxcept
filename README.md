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

## Contexts

- On hosted builds, every thread has its own try blocks : an error raised in
  a thread is caught by that thread's innermost try block.
- On Cortex-M cores, the thread mode and the interrupt handlers share the try
  blocks, which are opened and closed with interrupts masked through
  [uatom](https://github.com/ThomasAUB/uatom)'s `CriticalSection`. An error
  must be caught by the context that raised it : a handler must not raise
  outside of its own try blocks, nor return with one still open.

## CMake

uatom is a submodule : clone with `--recursive`, or run
`git submodule update --init --recursive`.

```cmake
add_subdirectory(uxcept)
target_link_libraries(my_target uxcept)
```
