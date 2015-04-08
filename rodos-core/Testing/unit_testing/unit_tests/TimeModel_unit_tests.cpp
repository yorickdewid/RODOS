
/*
   RODOS Unit Tests for Class TimeModel
   Sven Müller
*/

#include "gtest/gtest.h"
#include "timemodel.h"

// Fake implementation
long long hwGetNanoseconds()
{
  return 42;
}

// CTest entry function
int TimeModel_unit_tests(int argc, char* argv[])
{
  return RUN_ALL_TESTS();
}

// Test class
class TimeModel_unit_tests : public ::testing::Test
{
protected:
};

// Tests

TEST_F(TimeModel_unit_tests, computeNextBeat)
{
  // computeNextBeat is used by Thread and TimeEvent

  // begin: the beats are currently planned to begin at "begin" TODO: Is that
  // right? I can't say, because computeNextBeat lacks documentation and
  // "begin" is used in two ways: 1. Thread uses it as the begin of the
  // previous beating 2. TimeEvent uses it as the currently planned begin for
  // the next beating. That should be made clear
  // >>> https://mantis.dlr.de/mantis/view.php?id=8258
  // period: beat happens every "period" nanoseconds TODO: It should be
  // pointed out by the documentation that "period" must not be 0
  // >>> https://mantis.dlr.de/mantis/view.php?id=8258
  // timeNow: current time
  TTime begin, period, timeNow;

  // begin > timeNow: (Next) beating is planned to begin in the future. That's
  // ok, just leave it that way:
  // next beat == begin
  begin = 33, period = 5, timeNow = 20;
  ASSERT_EQ(33, TimeModel::computeNextBeat(begin, period, timeNow));

  // begin == timeNow: (Next) beating is planned to begin just now. That's too
  // soon, delay it to one period later:
  // next beat == begin + period
  begin = 33, period = 5, timeNow = 33;
  ASSERT_EQ(33+5, TimeModel::computeNextBeat(begin, period, timeNow));

  // begin < timeNow: Next beat is planned to begin in the past. That's
  // not good, delay it to one period later. Additionally, if the last beat
  // consumed more full periods than just one, delay it to that many
  // additional periods later (i. e. computeNextBeat "adjusts" the beating
  // so that computation fits into it):
  // next beat == begin + period + round_down((timeNow-begin)/period))
  //           == begin + period + full periods that fit into (timeNow-begin)
  begin = 33, period = 5, timeNow = 60;
  ASSERT_EQ(33+5+25, TimeModel::computeNextBeat(begin, period, timeNow));
}

//EXPECT_EQ(42, NOW()); // trivial, but whatever
//  cout << TimeModel::calendar2LocalTime(2012, 9, 11, 15, 40, 32.503198870) << endl;
//  long years, months, days, hours, minutes;
//  double seconds;
//  TimeModel::localTime2Calendar(400693232503198870LL, years, months, days, hours, minutes, seconds);
//  cout << years << "-" << months << "-" << days << " " << hours << ":" << minutes << ":" << seconds << endl;
