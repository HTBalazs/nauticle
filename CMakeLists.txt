cmake_minimum_required(VERSION 2.8.9)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread -O3")
project(nauticle_project)






# Set CMake behavior
cmake_policy(SET CMP0004 NEW) #
# Get DOLFIN configuration data (DOLFINConfig.cmake must be in
# DOLFIN_CMAKE_CONFIG_PATH)
find_package(DOLFIN REQUIRED) #
if (EXISTS ${DOLFIN_USE_FILE})
  include(${DOLFIN_USE_FILE})

  # Default build type (can be overridden by user)
  if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING
      "Choose the type of build, options are: Debug MinSizeRel Release RelWithDebInfo." FORCE)
  endif()

  # Do not throw error for 'multi-line comments' (these are typical in
  # rst which includes LaTeX)
  include(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG("-Wno-comment" HAVE_NO_MULTLINE)
  if (HAVE_NO_MULTLINE)
    set(CMAKE_CXX_FLAGS "-Wno-comment ${CMAKE_CXX_FLAGS}")
  endif()

else()
  # Compiler definitions
  add_definitions(${DOLFIN_CXX_DEFINITIONS})

  # Compiler flags
  set(CMAKE_CXX_FLAGS "${DOLFIN_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")

  # Do not throw error for 'multi-line comments' (these are typical in
  # rst which includes LaTeX)
  include(CheckCXXCompilerFlag)
  CHECK_CXX_COMPILER_FLAG("-Wno-comment" HAVE_NO_MULTLINE)
  if (HAVE_NO_MULTLINE)
    set(CMAKE_CXX_FLAGS "-Wno-comment ${CMAKE_CXX_FLAGS}")
  endif()

  # Include directories
  include_directories(${DOLFIN_INCLUDE_DIRS})
  include_directories(SYSTEM ${DOLFIN_3RD_PARTY_INCLUDE_DIRS})
endif()






set(VTK_DIR /usr/local/VTK-7.0.0)
set(EXECUTABLE nauticle)

link_directories(/usr/local/lib/prolog)
link_directories(/usr/local/lib/commonutils)
link_directories(/usr/local/lib)

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

target_link_libraries(${EXEC} ${DOLFIN_LIBRARIES})
target_link_libraries(${EXEC} /usr/local/lib/prolog/libprolog.a)
target_link_libraries(${EXEC} /usr/local/lib/commonutils/libcommonutils.a)
target_link_libraries(${EXEC} /usr/local/lib/libyaml-cpp.a)
