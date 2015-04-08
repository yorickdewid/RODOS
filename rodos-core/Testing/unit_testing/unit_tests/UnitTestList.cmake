
# 
# RODOS Unit Test List
# 
# In here you are supposed to list the source files with your unit tests
# in it as well as what they test by using:
# 
# add_rodos_unit_tests(
#   UNIT_TEST_SOURCE_FILE
#   SOURCE_FILE_TO_UNIT_TEST
#   ...
#   SOURCE_FILE_TO_UNIT_TEST
# )
# 
# UNIT_TEST_SOURCE_FILE: path to source file with a unit test in it, 
#   relative to this directory
# SOURCE_FILE_TO_UNIT_TEST: path to a RODOS source file you want to unit test,
#   relative to RODOS source directory
# 
# Please also consult tutorial
# 

add_rodos_unit_tests(
  string_pico_unit_tests.cpp
  src/independent/string_pico.cpp
)


add_rodos_unit_tests(
  Application_unit_tests.cpp
  src/independent/application.cpp
  src/independent/listelement.cpp
)

add_rodos_unit_tests(
  TimeModel_unit_tests.cpp
  src/independent/timemodel.cpp
)

add_rodos_unit_tests(
  TimeEvent_unit_tests.cpp
  src/independent/timeevent.cpp
  src/independent/listelement.cpp
)

