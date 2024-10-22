#add_executable(dialogue_test dialogue_test.cpp)
#target_link_libraries(dialogue_test PRIVATE dialogue GTest::gtest GTest::gtest_main)
#gtest_discover_tests(dialogue_test)
#
#add_executable(address_t_test address_t_test.cpp)
#target_include_directories(address_t_test PRIVATE ${CMAKE_SOURCE_DIR}/src)
#target_link_libraries(address_t_test PRIVATE GTest::gtest GTest::gtest_main)
#gtest_discover_tests(address_t_test)
#
#add_executable(server_test server_test.cpp)
#target_link_libraries(server_test PRIVATE server GTest::gtest GTest::gtest_main)
#gtest_discover_tests(server_test)

#add_executable(hook_param_test)
#target_sources(hook_param_test
#        PUBLIC
#        FILE_SET CXX_MODULES
#        FILES
#        hook_param_test.ixx
#)
#
#target_link_libraries(hook_param_test PRIVATE neohooklibs GTest::gtest GTest::gtest_main)