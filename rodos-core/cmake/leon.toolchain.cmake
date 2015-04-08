IF(NOT DEFINED ARCH)
	message(FATAL_ERROR "Do not use this file directly!")
ENDIF()

# Maybe we should set a different compiler for windows
CMAKE_FORCE_C_COMPILER (sparc-v8-elf-gcc gnu)
#set (CMAKE_C_COMPILER sparc-v8-elf-gcc)
CMAKE_FORCE_CXX_COMPILER (sparc-v8-elf-g++ gnu)
#set (CMAKE_CXX_COMPILER sparc-v8-elf-g++)

project(${PROJECT_NAME})

set (CMAKE_C_FLAGS "-fno-exceptions -W -Wall -Wextra -Wno-unused-parameter -Wundef -Os -nostartfiles -nodefaultlibs -nostdlib -mcpu=v8 -mfpu -mapp-regs -fcall-used-g6 -fcall-used-g7 -frename-registers")
set (CMAKE_CXX_FLAGS "-fno-exceptions -W -Wall -Wextra -Wno-unused-parameter -Wundef -Os -nostartfiles -nodefaultlibs -nostdlib -mcpu=v8 -mfpu -mapp-regs -fcall-used-g6 -fcall-used-g7 -frename-registers -fno-rtti -fno-exceptions -fno-unwind-tables -fno-asynchronous-unwind-tables")
set (LINKER_SCRIPT "${PROJECT_ROOT_DIR}/libs/${ARCH}/linkerscript.L")
set (LINKER_FLAGS "-Wl,--script=${LINKER_SCRIPT} -nostartfiles -nodefaultlibs -nostdlib")
set (APP_LIBRARIES m gcc)

set (SRC_DIR_LIST 	independent
			bare-metal-generic
			bare-metal/leon/${ARCH}
			bare-metal/leon
	)

#Create linkerscript
#this way it's build every time. there are ways to build only if needed but they require a dummy file and stuff so this is the simple way
add_custom_target (linkerscript ALL ${CMAKE_CXX_COMPILER} -x c++ -Wp,-P -I"${PROJECT_ROOT_DIR}/src/bare-metal/leon" -I"${PROJECT_ROOT_DIR}/src/bare-metal/leon/${ARCH}" -E "${PROJECT_ROOT_DIR}/src/bare-metal/leon/linkerscript.L" >linkerscript.L WORKING_DIRECTORY "${PROJECT_ROOT_DIR}/libs/${ARCH}")

message (STATUS "${ARCH} toolchain loaded")
