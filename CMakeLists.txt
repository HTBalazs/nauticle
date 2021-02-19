cmake_minimum_required(VERSION 2.8.9)
project(nauticle_project)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread -O3 -fPIC")
set(CMAKE_BUILD_TYPE Release)

set(VTK_DIR ~/local/VTK-7.0.0)
set(EXECUTABLE nauticle)

link_directories(~/local/lib/prolog)
link_directories(~/local/lib/commonutils)
link_directories(~/local/lib/c2c)
link_directories(~/local/lib)

include_directories(~/local/include)
include_directories(/usr/local/include)

find_package(VTK REQUIRED)

IF(VTK_FOUND)
  message("found VTK. Version:" ${VTK_VERSION}. VTK_DIR: ${VTK_LIBRARIES})
ELSE(VTK_FOUND)
  message(FATAL_ERROR "Cannot build the executable without VTK. Please set the VTK_DIR")
ENDIF(VTK_FOUND)

#Bring the headers into the project
include(${VTK_USE_FILE})

#Look for source and header files recursively in the src/ directory
file(GLOB_RECURSE SOURCES src/*.cpp)
file(GLOB_RECURSE HEADERS src/*.h)

#Generate list of header files and include them
set (HEADERS_DIRS "")
foreach (_headerFile ${HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND HEADERS_DIRS ${_dir})
endforeach()
list(REMOVE_DUPLICATES HEADERS_DIRS)
include_directories(${HEADERS_DIRS})

#List header files
message(${HEADERS_DIRS})

# Check OS
if(WIN32)
	message(FATAL_ERROR "Nauticle is not supported on Windows.")
endif()
if(UNIX AND NOT APPLE)
	message("Building on Linux machine.")
	set(BINDIR bin/Linux)
elseif(APPLE)
	message("Building on Apple machine.")	
	set(BINDIR bin/Mac)
endif()

#Create directory for executable
file(MAKE_DIRECTORY ${BINDIR})
#Set executable path
set(EXEC ${BINDIR}/${EXECUTABLE})

#add_custom_target(build-time-make-directory ALL COMMAND ${CMAKE_COMMAND} -E make_directory ${BINDIR})

#Show executable path
message("Build dir: " ${EXEC})

add_executable(${EXEC} ${SOURCES})

#Bring dependencies
# VTK library
if(VTK_LIBRARIES)
	target_link_libraries(${EXEC} ${VTK_LIBRARIES})
endif()

target_link_libraries(${EXEC} /usr/local/lib/libyaml-cpp.a)
target_link_libraries(${EXEC} prolog)
target_link_libraries(${EXEC} commonutils)
target_link_libraries(${EXEC} c2c)

# Define library and include directories (do not modify the installation directories below)
set(STATIC_LIB_DIR ~/local/lib/nauticle)
set(INCLUDE_DIR ~/local/include/nauticle)
# generate libnauticle.a file
add_library(nauticle STATIC ${SOURCES})
# install lib and include directories
install(TARGETS nauticle DESTINATION ${STATIC_LIB_DIR})
install(FILES ${HEADERS} DESTINATION ${INCLUDE_DIR})
