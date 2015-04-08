set (ARCH stm32)

CMAKE_FORCE_C_COMPILER (arm-none-eabi-gcc gnu)
CMAKE_FORCE_CXX_COMPILER (arm-none-eabi-g++ gnu)

project(${PROJECT_NAME})


if(NOT DEFINED DENSITY)
	set (DENSITY MD)
endif()
if(NOT ((DENSITY MATCHES "MD") OR (DENSITY MATCHES "HD") OR (DENSITY MATCHES "CL")))
	message (FATAL "Invalid density specified!")
endif()

set (CMAKE_C_FLAGS "-fno-exceptions -Wno-unused-parameter -Os -pipe -mcpu=cortex-m3 -mthumb   -msoft-float -DSTM32F10X_${DENSITY}")
set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti -fno-exceptions")

set(LINKER_FLAGS "-T${PROJECT_ROOT_DIR}/src/bare-metal/stm32/link_stm32_${DENSITY}.ld -fno-unwind-tables -fno-asynchronous-unwind-tables --entry ResetISR")
set(APP_LIBRARIES c)

set (SRC_DIR_LIST 	independent
			independent/gateway
			bare-metal-generic
			bare-metal/${ARCH}
			bare-metal/${ARCH}/stm32
	)






