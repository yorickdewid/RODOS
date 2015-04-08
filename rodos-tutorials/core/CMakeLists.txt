SET (INCLUDED_APP
		preemptiontest.cpp
		timeevents.cpp
	)

ADD_EXECUTABLE(coreApps-${ARCH} ${INCLUDED_APP})

if(DEFINED LINKER_FLAGS)
	set_target_properties (coreApps-${ARCH} PROPERTIES LINK_FLAGS ${LINKER_FLAGS})
endif()

target_link_libraries (coreApps-${ARCH} rodos ${APP_LIBRARIES})
