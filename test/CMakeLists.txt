include(GoogleTest)

add_executable(pathfinder_test
        Grid_test.cpp
        BezierCurve_test.cpp
        FileLoader_test.cpp
)

target_link_libraries(pathfinder_test PRIVATE GTest::GTest GTest::gtest_main Model Service)

gtest_discover_tests(pathfinder_test)
