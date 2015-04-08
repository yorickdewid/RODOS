
# 
# RODOS Unit Testing CMake File
# 
# * Adds Google Test as subdirectory which creates Google Test lib targets
# * Combines Google Test lib and all unit tests declared in 
#   unit_tests/UnitTestList.cmake into on exe target - the unit test driver
# * Adds tests so that CTest properly calls the unit test driver
# 
# Consult tutorial for info on how to create and run tests.
# 

message(STATUS "Configuring unit testing")

# Set needed variables (especially GTEST_SOURCE_DIR which is set by user)
set(GTEST_SOURCE_DIR "" CACHE STRING "Path to Google Test source code.\nMust be an absolute path")
set(GTEST_INCLUDE_DIR ${GTEST_SOURCE_DIR}/include)
set(GTEST_INCLUDE_DIRECTIVE "gtest/gtest.h")
set(GTEST_LIB_TARGET gtest)
set(GTEST_BUILD_DIR ${PROJECT_BINARY_DIR}/Testing/gtest)
set(UNIT_TEST_DIR unit_tests)
set(UNIT_TEST_LIST_FILE ${UNIT_TEST_DIR}/UnitTestList.cmake)
set(UNIT_TEST_DRIVER_EXE_TARGET unit_test_driver)
set(UNIT_TEST_DRIVER_SOURCE_FILE ${UNIT_TEST_DRIVER_EXE_TARGET}.cpp)

# Validate GTEST_SOURCE_DIR
if(GTEST_SOURCE_DIR STREQUAL "")
  error("GTEST_SOURCE_DIR must be set")
endif()
if(NOT IS_ABSOLUTE ${GTEST_SOURCE_DIR})
  error("GTEST_SOURCE_DIR must be an absolute path")
endif()
if(NOT IS_DIRECTORY ${GTEST_SOURCE_DIR})
  error("GTEST_SOURCE_DIR must be an existing directory")
endif()
if(NOT EXISTS ${GTEST_SOURCE_DIR}/CMakeLists.txt)
  error("GTEST_SOURCE_DIR must name a directory with a CMakeLists.txt file in it - the Google Test CMakeLists.txt file, in fact (though I can't check for that)")
endif()

# Add Google Test and check if that worked
message(STATUS "Configuring Google Test (${GTEST_SOURCE_DIR})")
add_subdirectory(${GTEST_SOURCE_DIR} ${GTEST_BUILD_DIR})
message(STATUS "Configuring Google Test done")
if (NOT TARGET ${GTEST_LIB_TARGET})
  error("Configuring Google Test doesn't seem to have worked: Google Test lib target \"${GTEST_LIB_TARGET}\" doesn't exist")
endif()

# Hide Google Test cache variables in CCMake and CMake GUI
mark_as_advanced(FORCE BUILD_SHARED_LIBS)
mark_as_advanced(FORCE gtest_build_samples)
mark_as_advanced(FORCE gtest_build_tests)
mark_as_advanced(FORCE gtest_disable_pthreads)
mark_as_advanced(FORCE gtest_force_shared_crt)

# Create macro "add_rodos_unit_tests" and include unit test list file
# (unit_tests/UnitTestList.cmake)
# 
# With this macro, users declare their unit tests in the unit test list
# file. This results in two list variables being filled: 
# UNIT_TEST_SOURCE_FILES and SOURCE_FILES_TO_UNIT_TEST. Duplicate
# entries in these lists are prevented.
# Consult unit test list file for details on the macro's usage.
macro(add_rodos_unit_tests UNIT_TEST_SOURCE_FILE)
  list(APPEND UNIT_TEST_SOURCE_FILES ${UNIT_TEST_SOURCE_FILE})
  unset(UNIT_TEST_SOURCE_FILE)
  set(ARGS_WITHOUT_FIRST_ARG ${ARGV})
  list(REMOVE_AT ARGS_WITHOUT_FIRST_ARG 0)
  foreach(SOURCE_FILE_TO_UNIT_TEST ${ARGS_WITHOUT_FIRST_ARG})
    list(APPEND SOURCE_FILES_TO_UNIT_TEST ${SOURCE_FILE_TO_UNIT_TEST})
  endforeach()
  unset(ARGS_WITHOUT_FIRST_ARG)
  unset(SOURCE_FILE_TO_UNIT_TEST)
  list(REMOVE_DUPLICATES UNIT_TEST_SOURCE_FILES)
  list(REMOVE_DUPLICATES SOURCE_FILES_TO_UNIT_TEST)
endmacro()
include(${UNIT_TEST_LIST_FILE})

# For debugging purposes:
#message(STATUS "Added uinit test source files:")
#foreach(TEST_SOURCE_FILE ${UNIT_TEST_SOURCE_FILES})
#  message(STATUS " " ${UNIT_TEST_SOURCE_FILE})
#endforeach()
#message(STATUS "Added source files to unit test: ")
#foreach(SOURCE_FILE_TO_TEST ${SOURCE_FILES_TO_UNIT_TEST})
#  message(STATUS " " ${SOURCE_FILE_TO_UNIT_TEST})
#endforeach()

# Google Test needs to be initiated before a test can be run. So: Tell CMake
# to put that into the test driver source file that will be generated
set(CMAKE_TESTDRIVER_BEFORE_TESTMAIN
  "\n/* Added via CMake */\n::testing::InitGoogleTest(&ac, av);\n"
)

# Generate unit test driver source file
# The command creates ${UNIT_TEST_DRIVER_SOURCE_FILE} so that it expects
# a CTest entry function for each unit test source file (it forward-
# declares them). Particulary, it expects the signatures as
# "int <unit test source file without extension>(int, char*[])".
# The test driver has a main function which
# calls a CTest entry function according to the test driver's second
# call parameter.
# The command used for generation also inserts the gtest include
# directive, creates the list UNIT_TEST_SOURCE_FILES_PLUS_DRIVER and
# fills it what its name says.
message(STATUS "Generating ${UNIT_TEST_DRIVER_SOURCE_FILE}")
create_test_sourcelist(
  UNIT_TEST_SOURCE_FILES_PLUS_DRIVER
  ${UNIT_TEST_DRIVER_SOURCE_FILE}
  ${UNIT_TEST_SOURCE_FILES}
  EXTRA_INCLUDE
  ${GTEST_INCLUDE_DIRECTIVE}
)

# Make names of source files with unit tests in them absolute as they are
# named relative to ./unit_tests by the user and needed absolute by
# "add_executable".
# This has to be done AFTER generating the test driver. Otherwise the
# test driver would expect absolute file names as CTest entry function names
# (compare explanation above).
set(UNIT_TEST_SOURCE_FILES_COPY ${UNIT_TEST_SOURCE_FILES})
unset(UNIT_TEST_SOURCE_FILES)
foreach(UNIT_TEST_SOURCE_FILE ${UNIT_TEST_SOURCE_FILES_COPY})
  list(APPEND UNIT_TEST_SOURCE_FILES ${UNIT_TEST_DIR}/${UNIT_TEST_SOURCE_FILE})
endforeach()
set(UNIT_TEST_SOURCE_FILES_PLUS_DRIVER
  ${UNIT_TEST_SOURCE_FILES}
  ${UNIT_TEST_DRIVER_SOURCE_FILE}
)

# Make names of source files to unit test absolute as they are named relative
# to RODOS source dir by the user and needed absolute by "add_executable"
set(SOURCE_FILES_TO_UNIT_TEST_COPY ${SOURCE_FILES_TO_UNIT_TEST})
unset(SOURCE_FILES_TO_UNIT_TEST)
foreach(SOURCE_FILE_TO_UNIT_TEST ${SOURCE_FILES_TO_UNIT_TEST_COPY})
  list(APPEND
    SOURCE_FILES_TO_UNIT_TEST
    ${CMAKE_SOURCE_DIR}/${SOURCE_FILE_TO_UNIT_TEST}
  )
endforeach()

# Add include directory for Google Test and add the unit test driver target
include_directories(${GTEST_INCLUDE_DIR})
message(STATUS "Adding test driver")
add_executable(
  ${UNIT_TEST_DRIVER_EXE_TARGET}
  ${UNIT_TEST_SOURCE_FILES_PLUS_DRIVER}
  ${SOURCE_FILES_TO_UNIT_TEST}
)
target_link_libraries(${UNIT_TEST_DRIVER_EXE_TARGET} ${GTEST_LIB_TARGET})

# Last but not least: Add tests so that CTest will properly call the
# unit test driver afterwards when users invoke it to run the tests
message(STATUS "Adding CTest tests")
foreach(
  UNIT_TEST_SOURCE_FILE
  ${UNIT_TEST_SOURCE_FILES}
)
  get_filename_component(
    UNIT_TEST_SOURCE_FILE_WITHOUT_EXT
    ${UNIT_TEST_SOURCE_FILE}
    NAME_WE
  )
  add_test(${UNIT_TEST_SOURCE_FILE_WITHOUT_EXT} # <class or function group>_unit_tests
    ${PROJECT_BINARY_DIR}/bin/${UNIT_TEST_DRIVER_EXE_TARGET} # Place of unit
      # test driver exe
    ${UNIT_TEST_SOURCE_FILE_WITHOUT_EXT} # So that the test driver calls the CTest entry function
      # for the specified unit test name
    --gtest_filter=${UNIT_TEST_SOURCE_FILE_WITHOUT_EXT}.* # So that Google Test doesn't run all
      # tests but only the one specified
  )
endforeach()

message(STATUS "Configuring unit testing done")

