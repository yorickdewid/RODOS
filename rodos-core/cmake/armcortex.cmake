set (ARCH arm_cortex)

# Force the compilers to gcc for arm
# Maybe we should set a different compiler for windows (mingw?)
CMAKE_FORCE_C_COMPILER (arm-none-eabi-gcc gnu)
CMAKE_FORCE_CXX_COMPILER (arm-none-eabi-g++ gnu)

project(${PROJECT_NAME})

set (CMAKE_C_FLAGS "-mthumb -mcpu=cortex-m3 -msoft-float -Wno-unused-parameter -nostartfiles -nodefaultlibs -nostdlib -fno-unwind-tables -fno-asynchronous-unwind-tables -mapcs-frame -MD -Os -D inline=  -ffunction-sections -fdata-sections -Wno-write-strings")

set ( CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -D CM3=1 -fno-exceptions  -fno-rtti")

#add_definitions (-mthumb -mcpu=cortex-m3)

set (LINKER_SCRIPT "${PROJECT_ROOT_DIR}/libs/${ARCH}/linkerscript")
set (LINKER_FLAGS "-Wl,--script='${LINKER_SCRIPT}' -Wl,--gc-sections")
set (APP_LIBRARIES m gcc g stdc++ gcc)


set (SRC_DIR_LIST 	independent
			independent/gateway
			bare-metal-generic
			bare-metal/${ARCH}
			bare-metal/${ARCH}/src
			bare-metal/${ARCH}/utils
			bare-metal/${ARCH}/sd_fatfs/src
			bare-metal/${ARCH}/sd_fatfs/port
			bare-metal/${ARCH}/udp_lwip/port
			bare-metal/${ARCH}/udp_lwip/src/core
			bare-metal/${ARCH}/udp_lwip/src/core/ipv4
			bare-metal/${ARCH}/udp_lwip/src/netif
			bare-metal/${ARCH}/udp_lwip/src/include
			bare-metal/${ARCH}/udp_lwip/src/include/ipv4
	)

set (ARCH_INCLUDE_DIRECTORIES
 		bare-metal/${ARCH}/inc
 		bare-metal/${ARCH}/udp_lwip/src/include
 		bare-metal/${ARCH}/udp_lwip/src/include/ipv4
 	)


add_custom_target (linkerscript ALL ${CMAKE_CXX_COMPILER} -x c++ -DENTRY=ResetISR -Wp,-P -E "${PROJECT_ROOT_DIR}/src/bare-metal/${ARCH}/scripts/linkerscript" >linkerscript WORKING_DIRECTORY "${PROJECT_ROOT_DIR}/libs/${ARCH}")

