include_directories(${PROJECT_ROOT_DIR}/support_libs)

# for CMAKE
set(LOGGING ON)

#
# TODO: put the following framework config variables into cmake gui (separate *.cmake file)
#
# for pre-processor
add_definitions(-DLOGGING)
# for logging framework configuration
add_definitions(-DLOG_APP_ID=424298)
add_definitions(-DLOG_THREAD_PRIO_CONTROL=50)
add_definitions(-DLOG_THREAD_PRIO_INPUT=100)
add_definitions(-DLOG_ITEM_MAX_C_STRING_LENGTH=4)
add_definitions(-DLOG_ITEM_MAX_APP_NAME_LENGTH=4)
add_definitions(-DLOG_ITEM_MAX_THREAD_NAME_LENGTH=4)
add_definitions(-DLOG_BUFFER_SIZE_INPUT=100)
add_definitions(-DLOG_BUFFER_SIZE_CONTROL_ITEM_RECEIVE=20)
add_definitions(-DLOG_LEVEL_DEBUG_VALUE=10)
add_definitions(-DLOG_LEVEL_INFO_VALUE=20)
add_definitions(-DLOG_LEVEL_WARN_VALUE=30)
add_definitions(-DLOG_LEVEL_ERROR_VALUE=40)
add_definitions(-DLOG_LEVEL_OFF_VALUE=50) 
add_definitions(-DLOG_META_LEVEL_VALUE=50) # set to off
add_definitions(-DLOG_OUTPUT)
add_definitions(-DLOG_BUFFER_SIZE_ITEM_RECEIVE=100)
add_definitions(-DLOG_APP_ID_OUTPUT=424299)
add_definitions(-DLOG_THREAD_PRIO_SIMPLE_CONTROLLER=50)
add_definitions(-DLOG_THREAD_PRIO_OUTPUT=100)

SET (INCLUDED_APP
  batteryinterface.cpp
  powercontrol.cpp
  groundstationlogger.cpp
  example_topics.cpp
  ${PROJECT_ROOT_DIR}/support_libs/random.cpp
)

ADD_EXECUTABLE(monitoring-${ARCH} ${INCLUDED_APP})

if(DEFINED LINKER_FLAGS)
	set_target_properties (monitoring-${ARCH} PROPERTIES LINK_FLAGS ${LINKER_FLAGS})
endif()

target_link_libraries (monitoring-${ARCH} rodos ${APP_LIBRARIES})


