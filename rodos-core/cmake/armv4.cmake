set (ARCH arm_v4)

 	
# Force the compilers to gcc for arm
# Maybe we should set a different compiler for windows (mingw?)
CMAKE_FORCE_C_COMPILER (arm-none-eabi-gcc gnu)
CMAKE_FORCE_CXX_COMPILER (arm-none-eabi-g++ gnu)

project(${PROJECT_NAME})

#set (HWLDFLAGS "-nostartfiles -nodefaultlibs -nostdlib")
set (HWCFLAGS "-mcpu=arm7tdmi -mthumb-interwork -msoft-float -nostartfiles -nodefaultlibs -nostdlib -fno-exceptions -Wno-long-long -fno-unwind-tables -fno-asynchronous-unwind-tables")
set (CMAKE_C_FLAGS "-fno-exceptions -mapcs-frame -Wall -pedantic -Wextra -Wno-unused-parameter -Wno-long-long -Os ${HWCFLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti")

set (LINKER_SCRIPT "${PROJECT_ROOT_DIR}/src/bare-metal/${ARCH}/linkerscript.ld")
#set (LINKER_SCRIPT "${PROJECT_ROOT_DIR}/libs/${ARCH}/linkerscript")
set (LINKER_FLAGS "-Wl,--script='${LINKER_SCRIPT}' -nostartfiles -nodefaultlibs -nostdlib")
set (APP_LIBRARIES m gcc)

set (SRC_DIR_LIST 	independent
			independent/gateway
			bare-metal-generic
			bare-metal/${ARCH}
			bare-metal/${ARCH}/board
			bare-metal/${ARCH}/uart
	)


#add_custom_target (linkerscript ALL ${CMAKE_CXX_COMPILER} -x c++ -Wp,-P -E "${PROJECT_ROOT_DIR}/src/bare-metal/${ARCH}/linkerscript.ld" >linkerscript WORKING_DIRECTORY "${PROJECT_ROOT_DIR}/libs/${ARCH}")
