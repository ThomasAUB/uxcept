set(UXCEPT_UNIT_TESTS uxcept_tests)

include(CTest)

file(GLOB TARGET_SRC "./tests.cpp" )

add_executable(${UXCEPT_UNIT_TESTS} ${TARGET_SRC})

find_package(Threads REQUIRED)

target_link_libraries(${UXCEPT_UNIT_TESTS} uxcept Threads::Threads)

add_test(${UXCEPT_UNIT_TESTS} ${UXCEPT_UNIT_TESTS})