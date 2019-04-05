cmake_minimum_required(VERSION 3.12)
project(Smart_Control)
add_executable(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfatal-errors")

add_definitions(-DWINVER=0x0501)
add_definitions(-D_WIN32_WINNT=0x0501)

# Add cmake_modules path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

if (WIN32)
    set(SFML_STATIC_LIBRARIES TRUE)
    set(TGUI_STATIC_LIBRARIES TRUE)

    if (NOT DEFINED $ENV{TGUI_DIR})
        set(TGUI_ROOT
                "C:/Dropbox/TGUI-0.8.2/lib/cmake/TGUI"
                "D:/Dropbox/TGUI-0.8.2/lib/cmake/TGUI")
        set(TGUI_DIR TGUI_ROOT)
    endif ()

    if (NOT DEFINED $ENV{SFML_DIR})
        set(SFML_DIR
                "C:/Dropbox/SFML-2.5.1/lib/cmake/SFML"
                "D:/Dropbox/SFML-2.5.1/lib/cmake/SFML")
    endif ()
else ()
    if (NOT DEFINED $ENV{TGUI_ROOT})
        set(TGUI_ROOT "/home/michal/tgui-0.8/lib/cmake/TGUI")
    endif ()

    if (NOT DEFINED $ENV{SFML_DIR})
        set(SFML_DIR "/home/michal/sfml/lib/cmake/SFML")
    endif ()

endif ()

#Find any version 0.8 of tgui
#See the FindSFML.cmake file for additional details and instructions
find_package(TGUI 0.8 REQUIRED)
include_directories(${TGUI_DIR}/../../../include)
target_link_libraries(${PROJECT_NAME} tgui)


#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
find_package(SFML 2 COMPONENTS graphics REQUIRED)
include_directories(${SFML_DIR}/../../../include)
target_link_libraries(${PROJECT_NAME} sfml-graphics)


# creates version.cpp using cmake script
# a custom target that is always built
add_custom_target(svnheader #DEPENDS svn_header
        BYPRODUCTS "${CMAKE_BINARY_DIR}/version.cpp"
        COMMAND ${CMAKE_COMMAND} -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/CVersion.cmake) # svn_header is nothing more than a unique string
add_dependencies(${PROJECT_NAME} svnheader)
target_sources(${PROJECT_NAME} PRIVATE "${CMAKE_BINARY_DIR}/version.cpp")

# check git branch name
find_package(Git)
execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
        OUTPUT_VARIABLE GIT_BRANCH
        ERROR_QUIET OUTPUT_STRIP_TRAILING_WHITESPACE)

# set EXECUTABLE_NAME
if (${GIT_BRANCH} MATCHES "master")
    set(EXECUTABLE_NAME "Smart_Control")
else ()
    set(EXECUTABLE_NAME "Smart_Control-${GIT_BRANCH}")
endif ()

string(REPLACE "/" "-" EXECUTABLE_NAME ${EXECUTABLE_NAME})
string(REPLACE "\\" "-" EXECUTABLE_NAME ${EXECUTABLE_NAME})

# Define sources and executable
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/src)

file(GLOB_RECURSE GENERATED_SOURCES ${CMAKE_SOURCE_DIR}/src/*.cpp)
file(GLOB_RECURSE GENERATED_HEADERS ${CMAKE_SOURCE_DIR}/include/*.h ${CMAKE_SOURCE_DIR}/src/*.h)

if (WIN32)
    list(FILTER GENERATED_HEADERS EXCLUDE REGEX ".*/Unix/.*")
    list(FILTER GENERATED_SOURCES EXCLUDE REGEX ".*/Unix/.*")
    file(GLOB_RECURSE GENERATED_RC_FILES ${CMAKE_SOURCE_DIR}/include/*.rc)
endif ()


if (UNIX)
    list(FILTER GENERATED_HEADERS EXCLUDE REGEX ".*/Win32/.*")
    list(FILTER GENERATED_SOURCES EXCLUDE REGEX ".*/Win32/.*")
    set(GENERATED_RC_FILES "")

    target_link_libraries(${PROJECT_NAME} "GLU")
    target_link_libraries(${PROJECT_NAME} "GL")
    target_link_libraries(${PROJECT_NAME} "pthread")
endif ()

target_sources(${PROJECT_NAME} PRIVATE main.cpp ${GENERATED_SOURCES} ${GENERATED_RC_FILES} ${GENERATED_HEADERS})
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${EXECUTABLE_NAME}")
set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
