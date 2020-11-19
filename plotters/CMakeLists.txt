message(STATUS " building plotters ... ")

file (GLOB UTILS 
    ${PROJECT_SOURCE_DIR}/utils/*.cc
)

ADD_EXECUTABLE(plot_energy_resolution     plot_energy_resolution.cc ${EXTRA_SOURCEFILES} ${EXTRA_HEADERFILES})
TARGET_LINK_LIBRARIES(plot_energy_resolution   Evtdict ${ROOT_LIBRARIES})

ADD_EXECUTABLE(plot_phantom_image     plot_phantom_image.cc ${EXTRA_SOURCEFILES} ${EXTRA_HEADERFILES})
TARGET_LINK_LIBRARIES(plot_phantom_image   Evtdict ${ROOT_LIBRARIES})

ADD_EXECUTABLE(plot_materials     plot_materials.cc ${EXTRA_SOURCEFILES} ${EXTRA_HEADERFILES})
TARGET_LINK_LIBRARIES(plot_materials   Evtdict ${ROOT_LIBRARIES})

ADD_EXECUTABLE(plot_straightness     plot_straightness.cc ${EXTRA_SOURCEFILES} ${EXTRA_HEADERFILES})
TARGET_LINK_LIBRARIES(plot_straightness   Evtdict ${ROOT_LIBRARIES})
