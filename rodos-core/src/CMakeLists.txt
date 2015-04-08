# SRC_DIR_LIST set by toolchain file

SET( RODOS_LIBS_SRC "" )

foreach(srcdir ${SRC_DIR_LIST})
	FILE(GLOB ALL_CXX_FILES "${srcdir}/*.cpp" "${srcdir}/*.CPP")
	FILE(GLOB ALL_C_FILES "${srcdir}/*.c" "${srcdir}/*.C")
	FILE(GLOB ALL_ASM_FILES "${srcdir}/*.S" "${srcdir}/*.s")
	
	SET_PROPERTY(SOURCE ${ALL_ASM_FILES} PROPERTY LANGUAGE CXX)
	set_source_files_properties(${ALL_C_FILES} PROPERTIES COMPILE_FLAGS "-std=gnu99") #Mainly to suppress c++-style comments warning
	set_source_files_properties(${ALL_CXX_FILES} PROPERTIES COMPILE_FLAGS "-std=gnu++98")
	LIST( APPEND RODOS_LIBS_SRC ${ALL_CXX_FILES} ${ALL_C_FILES} ${ALL_ASM_FILES} )
endforeach()

ADD_LIBRARY(rodos STATIC ${RODOS_LIBS_SRC})
