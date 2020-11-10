message(STATUS " building plotters ... ")

file (GLOB UTILS 
    ${PROJECT_SOURCE_DIR}/utils/*.cc
)

ADD_EXECUTABLE(plot_EnergyRes     plot_EnergyRes.cc ${EXTRA_SOURCEFILES} ${EXTRA_HEADERFILES})
TARGET_LINK_LIBRARIES(plot_EnergyRes   Evtdict ${ROOT_LIBRARIES})

ADD_EXECUTABLE(plot_EnergyRes_byThickness     plot_EnergyRes_byThickness.cc ${EXTRA_SOURCEFILES} ${EXTRA_HEADERFILES})
TARGET_LINK_LIBRARIES(plot_EnergyRes_byThickness   Evtdict ${ROOT_LIBRARIES})
