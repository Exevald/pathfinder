include(GoogleTest)

add_executable(pathfinder_test
        Grid_test.cpp
        BezierCurve_test.cpp
)

target_link_libraries(pathfinder_test PRIVATE GTest::GTest GTest::gtest_main Model)

gtest_discover_tests(pathfinder_test)
