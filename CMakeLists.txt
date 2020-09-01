# Check if cmake has the required version
cmake_minimum_required(VERSION 2.6.0 FATAL_ERROR)

set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

# Set name of our project
project(pCT)

option(WITH_GEANT4_UIVIS "Build pCT simulation with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/CMOS)
include_directories(${PROJECT_SOURCE_DIR}/include/SciDet)
include_directories(${PROJECT_SOURCE_DIR}/include/SiDet)
include_directories(${PROJECT_SOURCE_DIR}/include/Phantom)

file (GLOB EXTRA_SOURCEFILES 
    ${PROJECT_SOURCE_DIR}/src/CMOS/*.cc
    ${PROJECT_SOURCE_DIR}/src/SciDet/*.cc
    ${PROJECT_SOURCE_DIR}/src/SiDet/*.cc
    ${PROJECT_SOURCE_DIR}/src/Phantom/*.cc
)

file (GLOB EXTRA_HEADERFILES 
    ${PROJECT_SOURCE_DIR}/src/CMOS/*.hh
    ${PROJECT_SOURCE_DIR}/src/SciDet/*.hh
    ${PROJECT_SOURCE_DIR}/include/SiDet/*.hh
    ${PROJECT_SOURCE_DIR}/include/Phantom/*.hh
)


list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

set(LIBRARY_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-overloaded-virtual -Wno-shadow -Wno-unused-const-variable")
if (APPLE)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-private-field -Wno-pointer-bool-conversion")
endif (APPLE)

INCLUDE_DIRECTORIES(. ${CMAKE_CURRENT_SOURCE_DIR}/src)

list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )
MESSAGE(STATUS ${PROJECT_SOURCE_DIR}/cmake)

find_package (ROOT COMPONENTS MathCoreProut RIO Hist Tree Net TreePlayer Geom)
if (ROOT_FOUND)
    message(STATUS "${ROOT_LIBRARIES}")
else (ROOT_FOUND)
    message(FATAL "Unable to find ROOT")
endif (ROOT_FOUND)
include_directories (${ROOT_INCLUDE_DIR})

message(STATUS "C flags      : " ${CMAKE_C_FLAGS})
message(STATUS "CXX flags    : " ${CMAKE_CXX_FLAGS})
message(STATUS "Linker flags : " ${CMAKE_EXE_LINKER_FLAGS}\n)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/src)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/analysis)

ADD_EXECUTABLE(pCTsim                             pCTsimulation.cc ${EXTRA_SOURCEFILES} ${EXTRA_HEADERFILES})
TARGET_LINK_LIBRARIES(pCTsim                      Evtdict ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

install(FILES ${CMAKE_BINARY_DIR}/src/libEventDict_rdict.pcm 
	      ${CMAKE_BINARY_DIR}/src/libEventDict.rootmap 
              ${CMAKE_BINARY_DIR}/src/EventDict.cxx 
		DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/lib)
