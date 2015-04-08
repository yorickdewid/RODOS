ADD_DEFINITIONS(-m32 -O2 -fno-exceptions -mfpmath=387)

SET(ARCH posix)

project(${PROJECT_NAME})

SET(SRC_DIR_LIST 	independent
				independent/gateway
				on-posix
	)
	
set (APP_LIBRARIES m rt)
