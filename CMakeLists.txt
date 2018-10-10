cmake_minimum_required(VERSION 3.12)
project(Smart_Control)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfatal-errors")

# define a macro that helps defining an option
macro(sfml_set_option var default type docstring)
    if (NOT DEFINED ${var})
        set(${var} ${default})
    endif ()
    set(${var} ${${var}} CACHE ${type} ${docstring} FORCE)
endmacro()


# determine whether to create a debug or release build
sfml_set_option(CMAKE_BUILD_TYPE Release STRING "Choose the type of build (Debug or Release)")


# Add cmake_modules path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})


#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML 2 COMPONENTS graphics REQUIRED)
include_directories(${SFML_DIR}/../../../include)

set(TGUI_ROOT "C:/Dropbox/TGUI-0.6.10")
#Find any version 0.6 of tgui
#See the FindSFML.cmake file for additional details and instructions
find_package(TGUI 0.6 REQUIRED)
include_directories(${TGUI_INCLUDE_DIR})


# a custom target that is always built
add_custom_target(svnheader ALL
        DEPENDS svn_header ${CMAKE_CURRENT_SOURCE_DIR}/version.cpp.in) # svn_header is nothing more than a unique string

# creates version.cpp using cmake script
add_custom_command(OUTPUT svn_header ${CMAKE_CURRENT_BINARY_DIR}/version.cpp
        COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/CVersion.cmake)
#configure_file("${CMAKE_SOURCE_DIR}/version.cpp.in" "${CMAKE_BINARY_DIR}/version.cpp" @ONLY)
set(VERSION_SOURCES version.h "${CMAKE_BINARY_DIR}/version.cpp")


# Define sources and executable
set(EXECUTABLE_NAME "Smart_Control")
include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)

file(GLOB_RECURSE GENERATED_SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE GENERATED_HEADERS ${CMAKE_SOURCE_DIR}/include/*.h)
file(GLOB_RECURSE GENERATED_RC_FILES ${CMAKE_SOURCE_DIR}/include/*.rc)

add_executable(${EXECUTABLE_NAME} main.cpp ${VERSION_SOURCES} ${GENERATED_SOURCES} ${GENERATED_RC_FILES} ${GENERATED_HEADERS})
set_target_properties(${EXECUTABLE_NAME} PROPERTIES OUTPUT_NAME "${EXECUTABLE_NAME}-user")
set_property(TARGET ${EXECUTABLE_NAME} PROPERTY WIN32_EXECUTABLE true)

target_link_libraries(${EXECUTABLE_NAME} ${TGUI_LIBRARY} sfml-graphics)