set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32 -O0 -fno-exceptions -mfpmath=387")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -O0 -fno-exceptions -mfpmath=387")

SET(ARCH linux_x86)

project(${PROJECT_NAME})

IF(NOT UNIX)
	message(FATAL "Cross compiling from Windows to Unix currently not supported!")
ENDIF()

SET(SRC_DIR_LIST 	independent
				independent/gateway
				bare-metal-generic
				bare-metal/linux_x86)

SET (APP_LIBRARIES m rt)

IF(LOGGING)
    message(STATUS "logging framework integrated")
    SET(SRC_DIR_LIST    ${SRC_DIR_LIST}
                independent/log
                independent/log/meta)

    SET(APP_LIBRARIES   ${APP_LIBRARIES}
                support_libs)
ENDIF()
