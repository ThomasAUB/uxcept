#include <iostream>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "uxcept.hpp"

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

