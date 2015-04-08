set (ARCH avr32)
set (SYSTEM_NAME Generic)

CMAKE_FORCE_C_COMPILER (avr32-gcc gnu)
CMAKE_FORCE_CXX_COMPILER (avr32-g++ gnu)

project(${PROJECT_NAME})

set (CMAKE_C_FLAGS "-D__AVR32_UC3A0512__ -DBOARD=EVK1100 -fno-exceptions -Wno-unused-parameter -Wno-long-long -Os -pipe -Wall -g3 -gdwarf-2 -c -fmessage-length=0 -ffunction-sections -masm-addr-pseudos -mpart=uc3a0512")
set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS}")

set(LINKER_FLAGS "-T${PROJECT_ROOT_DIR}/src/bare-metal/avr32/scripts/link_uc3a0512.ld -Wl,--gc-sections --rodata-writable")
set(APP_LIBRARIES c m)

set (SRC_DIR_LIST 	independent
			#independent/gateway
			bare-metal-generic
			bare-metal/${ARCH}/
			bare-metal/${ARCH}/avr32_framework
			bare-metal/${ARCH}/hal
	)
	
#This does not work. I don't know how the person creating the port managed to compile it but I can't make it compile. There seem to be errors everywhere
	
