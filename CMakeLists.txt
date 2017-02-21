cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
project(LEMPS_project)

set(VTK_DIR /usr/local/VTK-7.0.0)
set(EXECUTABLE pmsimple)
link_directories(/usr/local/lib/prolog)
link_directories(/usr/local/lib/commonutils)
link_directories(/usr/local/lib/handyxml)

find_package(VTK REQUIRED)

IF(VTK_FOUND)
  message("found VTK. Version:" ${VTK_VERSION}. VTK_DIR: ${VTK_LIBRARIES})
ELSE(VTK_FOUND)
  message(FATAL_ERROR "Cannot build the executable without VTK. Please set the VTK_DIR")
ENDIF(VTK_FOUND)

#Bring the headers into the project
include(${VTK_USE_FILE})
include_directories(include)

#Add sources:
file(GLOB SOURCES "src/*.cpp")

if(WIN32)
	message(FATAL_ERROR "LEMPS is not supported on Windows.")
endif()

if(UNIX AND NOT APPLE)
	message("Building on Linux machine.")
	set(BINDIR bin/Linux)
elseif(APPLE)
	message("Building on Apple machine.")	
	set(BINDIR bin/Mac)
endif()

file(MAKE_DIRECTORY ${BINDIR})
set(EXEC ${BINDIR}/${EXECUTABLE})

add_custom_target(build-time-make-directory ALL COMMAND ${CMAKE_COMMAND} -E make_directory ${BINDIR})

message("Build dir: " ${EXEC})

add_executable(${EXEC} ${SOURCES})

# VTK library
if(VTK_LIBRARIES)
	target_link_libraries(${EXEC} ${VTK_LIBRARIES})
endif()

target_link_libraries(${EXEC} /usr/local/lib/prolog/libprolog.a)
target_link_libraries(${EXEC} /usr/local/lib/handyxml/libhandyxml.a)
target_link_libraries(${EXEC} /usr/local/lib/commonutils/libcommonutils.a)
