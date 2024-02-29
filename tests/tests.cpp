#include <iostream>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "uxcept.hpp"


void recursiveCheck(int v) {

    uxcept::tryCatch(

        [&] () {

            if (v == 0) {
                uxcept::raise("error : v equals 0 on enter");
            }
            else {

                recursiveCheck(v - 1);

                if (v == 2) {
                    std::cout << "v == 2" << std::endl;
                    uxcept::raise("error : v equals 2 on exit");
                }

            }

        },

        [&] (std::string_view inError) {
            if (v == 0) {
                CHECK(inError == "error : v equals 0 on enter");
            }
            else {
                CHECK(v == 2);
                CHECK(inError == "error : v equals 2 on exit");
            }
        }
    );
}


TEST_CASE("basic uxcept tests") {

    recursiveCheck(5);

}

