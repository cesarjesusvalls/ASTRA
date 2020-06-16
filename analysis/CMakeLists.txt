message(STATUS " building analysis targets ... ")

file (GLOB UTILS 
    ${PROJECT_SOURCE_DIR}/utils/*.cc
)

ADD_EXECUTABLE(templateAna          templateAna.cc ${EXTRA_SOURCEFILES}${EXTRA_HEADERFILES})
TARGET_LINK_LIBRARIES(templateAna   Evtdict ${ROOT_LIBRARIES})

ADD_EXECUTABLE(energyRes          energyRes.cc ${EXTRA_SOURCEFILES} ${EXTRA_HEADERFILES})
TARGET_LINK_LIBRARIES(energyRes   Evtdict ${ROOT_LIBRARIES})
