if(NOT DEFINED ARCH)
	set(ARCH windows)
endif()

set(SYSTEM_NAME Windows)

#ADD_DEFINITIONS(-m32 -O2 -fno-exceptions -mfpmath=387 -c)

project(${PROJECT_NAME})

SET(SRC_DIR_LIST	independent
			independent/gateway
			on-windows
	)

