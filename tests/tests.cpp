#include <iostream>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "uxcept.hpp"


TEST_CASE("basic uxcept tests") {

    auto crashTest =
        [&] (int v) {

        bool error = false;

        uxcept::tryCatch(
            [&] () {
                if (v < 0) {
                    uxcept::raise("error in crashTest");
                }
            },
            [&] (std::string_view inError) {
                error = true;
                CHECK(inError == "error in crashTest");
            }
        );

        return error;
        };

    CHECK(crashTest(1) == false);
    CHECK(crashTest(-1) == true);
}

