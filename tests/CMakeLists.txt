set(UXCEPT_UNIT_TESTS uxcept_tests)

include(CTest)

file(GLOB TARGET_SRC "./tests.cpp" )

add_executable(${UXCEPT_UNIT_TESTS} ${TARGET_SRC})

add_test(${UXCEPT_UNIT_TESTS} ${UXCEPT_UNIT_TESTS})