
#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(pCT)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build pCT simulation with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# lines to add ROOT libraries
list( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )
MESSAGE(STATUS ${PROJECT_SOURCE_DIR}/cmake)

find_package (ROOT COMPONENTS MathCoreProut RIO Hist Tree Net TreePlayer Geom)
if (ROOT_FOUND)
    message(STATUS "${ROOT_LIBRARIES}")
else (ROOT_FOUND)
    message(FATAL "Unable to find ROOT")
endif (ROOT_FOUND)
include_directories (${ROOT_INCLUDE_DIR})
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/CMOS)
include_directories(${PROJECT_SOURCE_DIR}/include/SciDet)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

file(GLOB CMOScc   ${PROJECT_SOURCE_DIR}/src/CMOS/*.cc)
file(GLOB CMOShh   ${PROJECT_SOURCE_DIR}/include/CMOS/*.hh)
file(GLOB SciDetcc ${PROJECT_SOURCE_DIR}/src/SciDet/*.cc)
file(GLOB SciDethh ${PROJECT_SOURCE_DIR}/include/SciDet/*.hh)

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(pCTsimulation pCTsimulation.cc ${sources} ${headers} ${CMOScc} ${CMOShh} ${SciDetcc} ${SciDethh})
target_link_libraries(pCTsimulation ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# remove plenty of warnings from ROOT source code
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-overloaded-virtual -Wno-shadow -Wno-unused-const-variable")
if (APPLE)
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-private-field -Wno-pointer-bool-conversion")
endif (APPLE)

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
add_custom_target(pCT DEPENDS pCTsimulation)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS pCTsimulation DESTINATION bin)


