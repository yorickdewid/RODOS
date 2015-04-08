
/*
   RODOS Unit Test for Function Group string_pico
   Sven Müller

   This file is also meant as template for creating new unit tests.
   Please consult README as well.
*/

// Google Test include
// Must be included before RODOS stuff
#include "gtest/gtest.h"

// RODOS includes needed for unit tests
#include "string_pico.h"

// CTest entry function
// Its name must be "<class or function group name>_unit_tests", just
// as the name of this file.
int string_pico_unit_tests(int argc, char* argv[])
{
  return RUN_ALL_TESTS(); // Normally, this runs all Google Test
    // tests. But CTest is configured to run only those which
    // belong to this class or function group.
}

// This is your unit test class (name also
// "<class or function group name>_unit_test"). Herein
// you can put data and functions which are shared among your tests, i. e.
// you can use this class's data and functions in all tests of your unit test.
// But note: Google Test constructs a FRESH unit test class for each test!
class string_pico_unit_tests : public ::testing::Test
{
protected:
  // Declare data and functions here
  const char* str;

  // Create data for your tests here
  string_pico_unit_tests()
  : str("Moin!")
  {
  }
  
  // Delete data for your tests here
  virtual ~string_pico_unit_tests()
  {
  }
};

// This is an actual unit test. The first parameter names, once again, your
// class or function group, the second names what is specifically tested.
// When choosing test names, keep two things in mind:
// 1st: Name the unit you want to test (class, function group, method or
// function name) and, if needed, the property you want to test.
// 2nd: test names are like C identifiers (perhaps, they are C
// identifiers, I don't know), i. e. they
// can't have white space in their names etc.
TEST_F(string_pico_unit_tests, strlen_does_it)
{
  // Basically, you make sure that the unit's code functions correctly by
  // making assertions about the code, i. e. what the code should assure.
  // For this purpose, there exist a bunch of ASSERT_* macros. See
  // https://code.google.com/p/googletest/wiki/Primer#Assertions for an
  // overview.
  
  // These are the most basic macros
  ASSERT_TRUE((size_t) 5 == strlen(str));
  ASSERT_FALSE((size_t) 4 == strlen(str));
  
  // EXPECT_EQ(expected_value, actual_value)
  // tests for equality (==)
  ASSERT_EQ((size_t) 5, strlen(str));
  
  // For more advanced stuff, have a look at
  // https://code.google.com/p/googletest/wiki/AdvancedGuide
  // or https://code.google.com/p/googletest/wiki/Documentation
  // in general.
}

// This is another unit test
TEST_F(string_pico_unit_tests, strlen_does_it_again)
{
  ASSERT_NE((size_t) 4, strlen(str));
}

// TODO: string_pico functions should be documented
// >>> https://mantis.dlr.de/mantis/view.php?id=8259
