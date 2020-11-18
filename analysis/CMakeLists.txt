message(STATUS " building analysis targets ... ")

file (GLOB UTILS 
    ${PROJECT_SOURCE_DIR}/utils/*.cc
)

ADD_EXECUTABLE(event_display          event_display.cc ${EXTRA_SOURCEFILES} ${EXTRA_HEADERFILES})
TARGET_LINK_LIBRARIES(event_display   Evtdict ${ROOT_LIBRARIES})

ADD_EXECUTABLE(energy_res          energy_res.cc ${EXTRA_SOURCEFILES} ${EXTRA_HEADERFILES})
TARGET_LINK_LIBRARIES(energy_res   Evtdict ${ROOT_LIBRARIES})

ADD_EXECUTABLE(advanced_event_display          advanced_event_display.cc ${EXTRA_SOURCEFILES} ${EXTRA_HEADERFILES})
TARGET_LINK_LIBRARIES(advanced_event_display   Evtdict ${ROOT_LIBRARIES})
