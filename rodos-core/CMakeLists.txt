# The name of our project is "RODOS". CMakeLists files in this project can
# refer to the root source directory of the project as ${RODOS_SOURCE_DIR} and
# to the root binary directory of the project as ${RODOS_BINARY_DIR}.
cmake_minimum_required (VERSION 2.6)
include (CMakeForceCompiler)
set (PROJECT_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set (CMAKE_MODULE_PATH "${PROJECT_ROOT_DIR}/cmake")
set (PROJECT_NAME "RODOS")
mark_as_advanced(FORCE CMAKE_BUILD_TYPE CMAKE_INSTALL_PREFIX) #To hide them in the CMake-GUI and ccmake

set(TOOLCHAIN_FILE "" CACHE STRING "Specifies the file that sets up the build environment.")

IF(TOOLCHAIN_FILE MATCHES ".+")
	message(STATUS "Toolchain file defined")
	set (CMAKE_SYSTEM_NAME Generic)
	INCLUDE(${TOOLCHAIN_FILE})
ELSE()
	message(STATUS "Not crosscompiling")
	INCLUDE(native.arch)
ENDIF()

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_ROOT_DIR}/libs/${ARCH}")
# general command line options for the compiler
IF ( CMAKE_COMPILER_IS_GNUCXX )
	add_definitions (-g -Wall -pedantic -Wno-long-long)	
ELSE ()
	add_definitions (-Wall)
ENDIF()

INCLUDE_DIRECTORIES(api/)
foreach(dir ${SRC_DIR_LIST})
	INCLUDE_DIRECTORIES(src/${dir})
endforeach()
foreach(dir ${ARCH_INCLUDE_DIRECTORIES})
	INCLUDE_DIRECTORIES(${dir})
endforeach()

ADD_SUBDIRECTORY(src)

option(LOGGING "Whether to turn the Logging Framework On/Off" OFF)
option(LOG_OUTPUT "Include logging framework output" OFF)
option(BUILD_SUPPORT_LIBS "Whether to build the support_libs" OFF)
option(BUILD_TUTORIALS "Whether to build the tutorials" OFF)
option(BUILD_SUPPORT_PROGRAMS "Whether to build the support_programs" OFF)
set(OWN_PROJECT "" CACHE STRING "Specifies a directory with a CMakeLists.txt file in it specifying your project.")

IF( BUILD_TUTORIALS )
	ADD_SUBDIRECTORY(tutorials ${PROJECT_BINARY_DIR}/tutorials)
ENDIF()

# Unit Testing
# See ./Testing/unit_testing/README
macro(error)
  message(FATAL_ERROR ${ARGV})
endmacro()
option(UNIT_TESTING_ENABLED "Whether to enable unit testing of RODOS" OFF)
if(UNIT_TESTING_ENABLED)
  enable_testing() # This must be in this very file (in RODOS root).
    # If not, CMake doesn't generate a CTestTestfile.cmake in RODOS' build
    # dir and ctest doesn't do anything when invoked from there
  message(STATUS "Unit testing enabled")
  add_subdirectory(Testing/unit_testing) # I use "T" rather than "t" since
    # CTest puts its log files into <RODOS build dir>/Testing/Temporary
else()
  message(STATUS "Unit testing disabled")
endif()

IF(BUILD_SUPPORT_LIBS OR LOGGING)
    include_directories(support_libs)
    add_subdirectory(support_libs)
ENDIF()

IF(BUILD_SUPPORT_PROGRAMS)
	ADD_SUBDIRECTORY(support_programs ${PROJECT_BINARY_DIR}/support_programs)
ENDIF()

IF(${OWN_PROJECT} MATCHES ".+")
	#message(STATUS "No other project defined.")
#IF(DEFINED OWN_PROJECT)
#ELSEIF()
	IF(IS_DIRECTORY ${PROJECT_BINARY_DIR}/${OWN_PROJECT})
		ADD_SUBDIRECTORY(${PROJECT_BINARY_DIR}/${OWN_PROJECT} ${EXECUTABLE_OUTPUT_PATH})
	ELSEIF(IS_DIRECTORY ${OWN_PROJECT})
		ADD_SUBDIRECTORY(${OWN_PROJECT} ${EXECUTABLE_OUTPUT_PATH})
	ELSEIF(IS_DIRECTORY ${PROJECT_ROOT_DIR}/${OWN_PROJECT})
		ADD_SUBDIRECTORY(${PROJECT_ROOT_DIR}/${OWN_PROJECT} ${EXECUTABLE_OUTPUT_PATH})
	ELSE()
		message(STATUS "Could not find OWN_PROJECT. Is it really a directory?")
	ENDIF()
ELSE()
	message(STATUS "No other project defined.")
ENDIF()

#
# global configuration of monitoring framework
# (to be overwritten by user specific options in CMakeLists.txt files)
#
IF(LOGGING)
    message(STATUS "Setting global Configuration of Logging Framework...")
    ADD_DEFINITIONS(-DLOGGING)
    
    SET(LOG_APP_ID 111111 CACHE STRING "Identification number of main logging framework application")
    MARK_AS_ADVANCED(FORCE LOG_APP_ID)
    ADD_DEFINITIONS(-DLOG_APP_ID=$(LOG_APP_ID))

    SET(LOG_THREAD_PRIO_CONTROL "50" CACHE STRING 
        "Execution priority for LogControlThread
        
         LogControlThread receives and realizes log controls.")
    MARK_AS_ADVANCED(FORCE LOG_THREAD_PRIO_CONTROL)
    ADD_DEFINITIONS(-DLOG_THREAD_PRIO_CONTROL=${LOG_THREAD_PRIO_CONTROL})

    SET(LOG_THREAD_PRIO_INPUT "100" CACHE STRING 
        "Execution priority for LogInputThread 
          
          LogInputThread collects and publishes log messages.")
    MARK_AS_ADVANCED(FORCE LOG_THREAD_PRIO_INPUT)
    ADD_DEFINITIONS(-DLOG_THREAD_PRIO_INPUT=${LOG_THREAD_PRIO_INPUT})

    SET(LOG_ITEM_MAX_C_STRING_LENGTH "4" CACHE STRING 
        "Maximum C string length that a log item can hold
          
         Every C string you log via logging framework is packed into a log item which is
         then send over network. If a C string is longer than LOG_ITEM_MAX_C_STRING_LENGTH,
         it is split up and distributed over as many log items as necessary.
         The higher this value, the bigger become the packets sent over network, but
         the less packets are sent in total.")
    MARK_AS_ADVANCED(FORCE LOG_ITEM_MAX_C_STRING_LENGTH)
    ADD_DEFINITIONS(-DLOG_ITEM_MAX_C_STRING_LENGTH=${LOG_ITEM_MAX_C_STRING_LENGTH})

    SET(LOG_ITEM_MAX_APP_NAME_LENGTH "4" CACHE STRING
        "Maximum application name length the LOG framework can handle
           
         Each and every log item is packed with the name of the application responsible
         for the corresponding input and then send over network. A too low value results
         in truncated application names when outputting log messages; a too high value
         results in too much network traffic.")
    MARK_AS_ADVANCED(FORCE LOG_ITEM_MAX_APP_NAME_LENGTH)
    ADD_DEFINITIONS(-DLOG_ITEM_MAX_APP_NAME_LENGTH=${LOG_ITEM_MAX_APP_NAME_LENGTH})

    SET(LOG_ITEM_MAX_THREAD_NAME_LENGTH "4" CACHE STRING 
        "Maximum thread name length the LOG framework can handle
        
        Each and every log item is packed with the name of the thread responsible
        for the corresponding input and then send over network. A too low value results
        in truncated thread names when outputting log messages; a too high value
        results in too much network traffic.")
    MARK_AS_ADVANCED(FORCE LOG_ITEM_MAX_THREAD_NAME_LENGTH)
    ADD_DEFINITIONS(-DLOG_ITEM_MAX_THREAD_NAME_LENGTH=${LOG_ITEM_MAX_THREAD_NAME_LENGTH})

  
    SET(LOG_BUFFER_SIZE_INPUT "100" CACHE STRING
        "Maximum number of items in the logging framework's input buffer

         Every single log input first goes through this buffer before RODOS
         distributes it over network. It's important to know that the input buffer 
         dismisses items if there is no free place left for them.")
    MARK_AS_ADVANCED(FORCE LOG_BUFFER_SIZE_INPUT)
    ADD_DEFINITIONS(-DLOG_BUFFER_SIZE_INPUT=${LOG_BUFFER_SIZE_INPUT})
  
    SET(LOG_BUFFER_SIZE_CONTROL_ITEM_RECEIVE "20" CACHE STRING
        "Maximum number of items in the logging framework's log control item receive buffer

         Log control items represent commands. When receiving such items from
         network, RODOS puts them into this buffer until there is time to process them.")
    MARK_AS_ADVANCED(FORCE LOG_BUFFER_SIZE_CONTROL_ITEM_RECEIVE)
    ADD_DEFINITIONS(-DLOG_BUFFER_SIZE_CONTROL_ITEM_RECEIVE=${LOG_BUFFER_SIZE_CONTROL_ITEM_RECEIVE})
  
    # CMake knows no enums
    SET(LOG_LEVEL_DEBUG debug)
    SET(LOG_LEVEL_INFO info)
    SET(LOG_LEVEL_WARN warn)
    SET(LOG_LEVEL_ERROR error)
    SET(LOG_LEVEL_OFF off)
    SET(
        LOG_LEVELS
        ${LOG_LEVEL_DEBUG}
        ${LOG_LEVEL_INFO}
        ${LOG_LEVEL_WARN}
        ${LOG_LEVEL_ERROR}
        ${LOG_LEVEL_OFF}
    )

    SET(LOG_LEVEL_DEBUG_VALUE "10" CACHE STRING
        "Value of '${LOG_LEVEL_DEBUG}' log/meta log level

         This level is compared to other levels based on this value.
         It may be necessary to adjust this value if you create your own log levels.")
    MARK_AS_ADVANCED(FORCE LOG_LEVEL_DEBUG_VALUE)
    ADD_DEFINITIONS(-DLOG_LEVEL_DEBUG_VALUE=${LOG_LEVEL_DEBUG_VALUE})

  
    SET(LOG_LEVEL_INFO_VALUE "20" CACHE STRING
        "Value of '${LOG_LEVEL_INFO}' log/meta log level

        This level is compared to other levels based on this value.
        It may be necessary to adjust this value if you create your own log levels.")
    MARK_AS_ADVANCED(FORCE LOG_LEVEL_INFO_VALUE)
    ADD_DEFINITIONS(-DLOG_LEVEL_INFO_VALUE=${LOG_LEVEL_INFO_VALUE})

    SET(LOG_LEVEL_WARN_VALUE "30" CACHE STRING
        "Value of '${LOG_LEVEL_WARN}' log/meta log level

        This level is compared to other levels based on this value.
        It may be necessary to adjust this value if you create your own log levels.")
    MARK_AS_ADVANCED(FORCE LOG_LEVEL_WARN_VALUE)
    ADD_DEFINITIONS(-DLOG_LEVEL_WARN_VALUE=${LOG_LEVEL_WARN_VALUE})

    SET(LOG_LEVEL_ERROR_VALUE "40" CACHE STRING
        "Value of '${LOG_LEVEL_ERROR}' log/meta log level

        This level is compared to other levels based on this value.
        It may be necessary to adjust this value if you create your own log levels.")
    MARK_AS_ADVANCED(FORCE LOG_LEVEL_ERROR_VALUE)
    ADD_DEFINITIONS(-DLOG_LEVEL_ERROR_VALUE=${LOG_LEVEL_ERROR_VALUE})

    SET(LOG_LEVEL_OFF_VALUE "50" CACHE STRING 
        "Value of '${LOG_LEVEL_OFF}' log/meta log level

        This level is compared to other levels based on this value.
        It may be necessary to adjust this value if you create your own log levels.")
    MARK_AS_ADVANCED(FORCE LOG_LEVEL_OFF_VALUE)
    ADD_DEFINITIONS(-DLOG_LEVEL_OFF_VALUE=${LOG_LEVEL_OFF_VALUE})

ENDIF() # if(LOGGING)

if(LOG_OUTPUT)
    ADD_DEFINITIONS(-DLOG_OUTPUT)
  
    IF(NOT BUILD_SUPPORT_LIBS)
        SET(BUILD_SUPPORT_LIBS TRUE)
    ENDIF()
  
    SET(LOG_BUFFER_SIZE_ITEM_RECEIVE "100" CACHE STRING
        "Maximum number of items in the logging framework's log item receive buffer

         Log items represent the things users log via Logger. When receiving such 
         items from network, RODOS puts them into this buffer until there is time 
         to process them.")
      MARK_AS_ADVANCED(FORCE LOG_BUFFER_SIZE_ITEM_RECEIVE)
      ADD_DEFINITIONS(-DLOG_BUFFER_SIZE_ITEM_RECEIVE=${LOG_BUFFER_SIZE_ITEM_RECEIVE})
  
    SET(LOG_APP_ID_OUTPUT "424299" CACHE STRING
        "Identification number of the logging framework's output application")
    MARK_AS_ADVANCED(FORCE LOG_APP_ID_OUTPUT)
    ADD_DEFINITIONS(-DLOG_APP_ID_OUTPUT=${LOG_APP_ID_OUTPUT})
 
    SET(LOG_THREAD_PRIO_SIMPLE_CONTROLLER "50" CACHE STRING
        "Execution priority for SimpleLogControllerThread

         SimpleLogControllerThread reads keyboard inputs, converts them into
         commands and publishes log control items in order to realize the commands.")
    MARK_AS_ADVANCED(FORCE LOG_THREAD_PRIO_SIMPLE_CONTROLLER)
    ADD_DEFINITIONS(-DLOG_THREAD_PRIO_SIMPLE_CONTROLLER=${LOG_THREAD_PRIO_SIMPLE_CONTROLLER})
  
    SET(LOG_THREAD_PRIO_OUTPUT "100" CACHE STRING
        "Execution priority for LogOutputThread

         LogOutputThread receives and outputs log messages")
    MARK_AS_ADVANCED(FORCE LOG_THREAD_PRIO_OUTPUT)
    ADD_DEFINITIONS(-DLOG_THREAD_PRIO_OUTPUT=${LOG_THREAD_PRIO_OUTPUT})
    
endif() # if(LOG_OUTPUT)

#ignore this. It's just here to prevent cmake from inserting one at the beginning of the file.
#this command has to be in every toolchain file after setting the compiler but BEFORE setting any compiler flags (because project() seems to reset them)
project(${PROJECT_NAME})


