cmake_minimum_required (VERSION 3.2)
project (test-GridLayout)

set(SOURCES
    test_allocsizes.cpp
    test_cellcenteredcoordinates.cpp
    test_deriv1d.cpp
    test_fieldnodecoordinates.cpp
    test_gridlayout.cpp
    test_indexing.cpp
    test_main.cpp
    test_utilities.cpp
    )





include_directories("./")
add_executable(Gridlayout ${SOURCES})
target_link_libraries(Gridlayout gtest gtest_main)
target_link_libraries(Gridlayout gmock gmock_main)
target_link_libraries(Gridlayout pharedata)

add_test(NAME test-GridLayout COMMAND Gridlayout)
add_custom_command(TARGET Gridlayout
                   POST_BUILD
                   COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/gridlayouttest.py ${CMAKE_CURRENT_BINARY_DIR})

