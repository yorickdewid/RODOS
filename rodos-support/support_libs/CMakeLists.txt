
# TODO: add other support libs

# for logging framework
IF(LOGGING)
  list(APPEND SUPPORT_LIBS_SRC
    log/LogOutputThread.cpp
    log/Layout.cpp
    log/DummyLayout.cpp
    log/PatternLayout.cpp
    log/Appender.cpp
    log/ConsoleAppender.cpp
    log/LogOutput.cpp
    log/SimpleLogControllerThread.cpp
    log/logOutputApp.cpp
  )
ENDIF()
 
add_library(support_libs STATIC ${SUPPORT_LIBS_SRC}) 
target_link_libraries(support_libs rodos)
