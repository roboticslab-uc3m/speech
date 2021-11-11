if(NOT GTestSources_FOUND AND (NOT DEFINED ENABLE_tests OR ENABLE_tests))
    message(WARNING "GTestSources package not found, disabling tests")
endif()

cmake_dependent_option(ENABLE_tests "Enable/disable unit tests" ON
                       GTestSources_FOUND OFF)

if(ENABLE_tests)

    add_subdirectory(${GTestSources_SOURCE_DIR} ${CMAKE_BINARY_DIR}/googletest)
    include_directories(${GTestSources_INCLUDE_DIR})

    # testEspeak

    if(ENABLE_Espeak)
        add_executable(testEspeak testEspeak.cpp)

        target_link_libraries(testEspeak YARP::YARP_os
                                         ROBOTICSLAB::Espeak
                                         gtest_main)

        gtest_discover_tests(testEspeak)
    endif()

else()

    set(ENABLE_tests OFF CACHE BOOL "Enable/disable unit tests" FORCE)

endif()
