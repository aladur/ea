set(unittests_SOURCES
    test_main.cpp
)
set(unittests_HEADER
    #test_main.h
)

add_executable(unittests ${unittests_SOURCES} ${unittests_HEADER})
target_include_directories(unittests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_BINARY_DIR}/src
)

target_include_directories(unittests SYSTEM PRIVATE
    ${gtest_SOURCE_DIR}/include
    ${gmock_SOURCE_DIR}/include
)

target_link_libraries(unittests PRIVATE
    ea::lib
    gtest
)

include(GoogleTest)
gtest_discover_tests(unittests DISCOVERY_MODE PRE_TEST)
