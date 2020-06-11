message(STATUS " building analysis targets ... ")

ADD_EXECUTABLE(templateAna          templateAna.cc ${EXTRA_SOURCEFILES} ${EXTRA_HEADERFILES})
TARGET_LINK_LIBRARIES(templateAna   Evtdict ${ROOT_LIBRARIES})
