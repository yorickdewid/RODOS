
/*
   RODOS Unit Tests for Class Application
   Sven Müller
*/

#include "gtest/gtest.h"
#include "application.h"

// Fake implementations
void PRINTF(const char* fmt, ...)
{
}
void ERROR(const char* text)
{
}
volatile long taskRunning = 0;

// CTest entry function
int Application_unit_tests(int argc, char* argv[])
{
  return RUN_ALL_TESTS();
}

// Test class
class Application_unit_tests : public ::testing::Test
{
protected:
  Application app0, app1, app2, app3;
  static const int DEFAULT_APP_ID = 1000;
  static const int APP0_ID = 9328;
  static const int APP2_ID = 2343;
  static const int NO_APP_ID = -533;

  Application_unit_tests()
  : app0("Application 0", APP0_ID), app1("Application 1"),
    app2("Application 2", APP2_ID), app3("Application 3")
  {
  }
};

// Tests

TEST_F(Application_unit_tests, findApplication)
{
  // Find app 2
  ASSERT_EQ(&app2, Application::findApplication(APP2_ID));

  // Find no app
  ASSERT_EQ(0, Application::findApplication(NO_APP_ID));

  // Find app 1 or app 3 (they have same ids)
  // TODO: It's unclear what findApplication is supposed to do
  // in this case >>> Mantis issue?!
  ASSERT_TRUE(&app1 == Application::findApplication(DEFAULT_APP_ID) ||
    &app3 == Application::findApplication(DEFAULT_APP_ID));
}

// TODO: Mocking needed to test printApplications
//TEST_F(Application_unit_tests, printApplications)
//{
//  Application::printApplications();
//}
