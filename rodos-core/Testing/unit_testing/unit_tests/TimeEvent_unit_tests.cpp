
/*
   RODOS Unit Tests for Class TimeEvent
   Sven Müller
*/

#include "gtest/gtest.h"
#include "timeevent.h"
#include "listelement.h"

List TimeEvent::timeEventList = 0;

int TimeEvent_unit_tests(int argc, char* argv[])
{
  return RUN_ALL_TESTS();
}

class TimeEvent_unit_tests : public ::testing::Test
{
protected:
  TimeEvent teSingle, tePeriodic;
  static const TTime BEGIN_SINGLE = 1000;
  static const TTime BEGIN_PERIODIC = 500;
  static const TTime PERIOD = 500;

  TimeEvent_unit_tests()
  : teSingle("Single"), tePeriodic("Periodic")
  {
    teSingle.activateAt(BEGIN_SINGLE);
    tePeriodic.activatePeriodic(BEGIN_PERIODIC, PERIOD);
  }
};

// TODO: Mock handle methods in order to test that they are invoked by
// propagate
TEST_F(TimeEvent_unit_tests, propagate)
{
  // Time = -1000: no change
  TimeEvent::propagate(-1000);
  ASSERT_EQ(BEGIN_SINGLE, teSingle.eventAt);
  ASSERT_EQ(0, teSingle.eventPeriod);
  ASSERT_EQ(BEGIN_PERIODIC, tePeriodic.eventAt);
  ASSERT_EQ(PERIOD, tePeriodic.eventPeriod);

  // Time = -1: no change
  TimeEvent::propagate(-1);
  ASSERT_EQ(BEGIN_SINGLE, teSingle.eventAt);
  ASSERT_EQ(0, teSingle.eventPeriod);
  ASSERT_EQ(BEGIN_PERIODIC, tePeriodic.eventAt);
  ASSERT_EQ(PERIOD, tePeriodic.eventPeriod);

  // Time = 0: no change
  TimeEvent::propagate(0);
  ASSERT_EQ(BEGIN_SINGLE, teSingle.eventAt);
  ASSERT_EQ(0, teSingle.eventPeriod);
  ASSERT_EQ(BEGIN_PERIODIC, tePeriodic.eventAt);
  ASSERT_EQ(PERIOD, tePeriodic.eventPeriod);

  // Time = 1: no change
  TimeEvent::propagate(1);
  ASSERT_EQ(BEGIN_SINGLE, teSingle.eventAt);
  ASSERT_EQ(0, teSingle.eventPeriod);
  ASSERT_EQ(BEGIN_PERIODIC, tePeriodic.eventAt);
  ASSERT_EQ(PERIOD, tePeriodic.eventPeriod);

  // Time = 100: no change
  TimeEvent::propagate(100);
  ASSERT_EQ(BEGIN_SINGLE, teSingle.eventAt);
  ASSERT_EQ(0, teSingle.eventPeriod);
  ASSERT_EQ(BEGIN_PERIODIC, tePeriodic.eventAt);
  ASSERT_EQ(PERIOD, tePeriodic.eventPeriod);

  // Time = 499: no change
  TimeEvent::propagate(499);
  ASSERT_EQ(BEGIN_SINGLE, teSingle.eventAt);
  ASSERT_EQ(0, teSingle.eventPeriod);
  ASSERT_EQ(BEGIN_PERIODIC, tePeriodic.eventAt);
  ASSERT_EQ(PERIOD, tePeriodic.eventPeriod);

  // Time = 500: TODO: It's unclear what propagate is supposed to do here.
  // I'd think it should do something with periodic event, but it
  // doesn't ("<=" instead of "<"?). If yes, then time = 501 has to be
  // updated >>> https://mantis.dlr.de/mantis/view.php?id=8253

  // Time = 501: periodic event fires, its eventAt is updated to a
  // time that's not in the past anymore TODO: What time exactly? It's unclear
  // why it's updated in that specific manner. It should be stated in
  // propagate's documentation
  // >>> https://mantis.dlr.de/mantis/view.php?id=8256
  TimeEvent::propagate(501);
  ASSERT_EQ(BEGIN_SINGLE, teSingle.eventAt);
  ASSERT_EQ(0, teSingle.eventPeriod);
  ASSERT_LE(501, tePeriodic.eventAt); // less than or equal
  ASSERT_EQ(PERIOD, tePeriodic.eventPeriod);

  // Time = 999: No change
  TimeEvent::propagate(999);
  ASSERT_EQ(BEGIN_SINGLE, teSingle.eventAt);
  ASSERT_EQ(0, teSingle.eventPeriod);
  ASSERT_LE(501, tePeriodic.eventAt); // less than or equal
  ASSERT_EQ(PERIOD, tePeriodic.eventPeriod);

  // Time = 1000: TODO: See time = 500

  // Time = 1001: propagate fires the single event and updates its eventAt to
  // END_OF_TIME TODO: This should be stated in propagate's documentation
  // >>> https://mantis.dlr.de/mantis/view.php?id=8256
  //
  // Periodic event fires, too, and its eventAt is changed to
  // a time that's not in the past TODO: See time = 501.
  TimeEvent::propagate(1001);
  ASSERT_EQ(END_OF_TIME, teSingle.eventAt);
  ASSERT_EQ(0, teSingle.eventPeriod);
  ASSERT_LE(501, tePeriodic.eventAt); // less than or equal
  ASSERT_EQ(PERIOD, tePeriodic.eventPeriod);

  // Time = 19247: propagate fires the periodic event again and updates its
  // eventAt to a time that's not in the past TODO: See time = 501.
  TimeEvent::propagate(19247);
  ASSERT_EQ(END_OF_TIME, teSingle.eventAt);
  ASSERT_EQ(0, teSingle.eventPeriod);
  ASSERT_LE(19247, tePeriodic.eventAt); // less than or equal
  ASSERT_EQ(PERIOD, tePeriodic.eventPeriod);
}
