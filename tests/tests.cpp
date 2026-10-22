#include <iostream>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "uxcept.hpp"

#include <atomic>
#include <thread>

static bool sFail_0 = false;
static bool sFail_1 = false;

static void recursiveCheck(int v) {

    uxcept::tryCatch(
        [&] () {
            if (v == 0) {
                uxcept::raise("error : v equals 0 on enter");
            }
            else {
                recursiveCheck(v - 1);
                if (v == 1) {
                    uxcept::raise("error : v equals 1 on exit");
                }
            }
        },
        [&] (uxcept::error_t inError) {
            if (v == 0) {
                sFail_0 = true;
                CHECK(inError == "error : v equals 0 on enter");
            }
            else if (v == 1) {
                sFail_1 = true;
                CHECK(v == 1);
                CHECK(inError == "error : v equals 1 on exit");
            }
            else {
                uxcept::raise("erf");
            }
        }
    );
}

TEST_CASE("basic uxcept tests") {
    recursiveCheck(5);
    CHECK(sFail_0);
    CHECK(sFail_1);
}

TEST_CASE("try blocks are per thread on hosted builds") {
    // Each thread raises from inside its own try block, while the other one
    // has a try block open : every error must land in its own thread's catch.
    constexpr int iterations = 10000;
    std::atomic<int> caught[2] = { 0, 0 };
    std::atomic<int> misrouted = 0;

    auto worker = [&] (int id) {
        for (int i = 0; i < iterations; ++i) {
            uxcept::tryCatch(
                [&] () {
                    uxcept::raise(id == 0 ? "thread 0" : "thread 1");
                },
                [&] (uxcept::error_t inError) {
                    if (inError == (id == 0 ? "thread 0" : "thread 1")) {
                        ++caught[id];
                    }
                    else {
                        ++misrouted;
                    }
                }
            );
        }
    };

    std::thread t0(worker, 0);
    std::thread t1(worker, 1);
    t0.join();
    t1.join();

    CHECK(caught[0] == iterations);
    CHECK(caught[1] == iterations);
    CHECK(misrouted == 0);
}
