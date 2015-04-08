set (ARCH ppc405_xupv2p)
set (CMAKE_SYSTEM_NAME Generic)

if (NOT DEFINED DIETLIBC)
	set (DIETLIBC "/opt/dietlibc-0.32")
endif()

CMAKE_FORCE_C_COMPILER (powerpc-405-linux-gnu-gcc gnu)
CMAKE_FORCE_CXX_COMPILER (powerpc-405-linux-gnu-g++ gnu)

project(${PROJECT_NAME})

set (CMAKE_C_FLAGS "-Wno-unused-parameter -ffreestanding -nostdlib -static -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables -O3 -nostdinc")
set (CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -fno-rtti")


set(LINKER_FLAGS "-Wl,--script=${PROJECT_ROOT_DIR}/src/bare-metal/ppc405_xupv2p/linkerscript")
set(APP_LIBRARIES gcc m)

set (SRC_DIR_LIST 	independent
			independent/gateway
			bare-metal-generic
			bare-metal/${ARCH}
	)

set (ARCH_INCLUDE_DIRECTORIES
 		${DIETLIBC}/include/
 	)


