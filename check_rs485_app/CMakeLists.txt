cmake_minimum_required(VERSION 3.12)
project(check_rs485_app)
add_executable(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfatal-errors")

add_definitions(-DWINVER=0x0501)
add_definitions(-D_WIN32_WINNT=0x0501)

# Add cmake_modules path
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})
if (WIN32)
    set(SFML_STATIC_LIBRARIES TRUE)

    if (NOT DEFINED $ENV{TGUI_ROOT})
        set(TGUI_ROOT
                "C:/Dropbox/TGUI-0.6.10"
                "D:/Dropbox/TGUI-0.6.10")
    endif ()
else ()
    set(TGUI_ROOT "/home/michal/tgui")
endif ()

#Find any version 0.6 of tgui
#See the FindSFML.cmake file for additional details and instructions
find_package(TGUI 0.6 REQUIRED)
include_directories(${TGUI_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${TGUI_LIBRARY})


#Find any version 2.X of SFML
#See the FindSFML.cmake file for additional details and instructions
find_package(SFML 2 COMPONENTS graphics REQUIRED)
include_directories(${SFML_DIR}/../../../include)
target_link_libraries(${PROJECT_NAME} sfml-graphics)


# Define include folders of our target
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Define sources and executable
set(GENERATED_SOURCES_FOLDER_LIST
        "${CMAKE_CURRENT_SOURCE_DIR}/../src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

set(GENERATED_HEADERS_FOLDER_LIST
        "${CMAKE_CURRENT_SOURCE_DIR}/../include/*.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

file(GLOB_RECURSE GENERATED_SOURCES ${GENERATED_SOURCES_FOLDER_LIST})
file(GLOB_RECURSE GENERATED_HEADERS ${GENERATED_HEADERS_FOLDER_LIST})

list(FILTER GENERATED_HEADERS EXCLUDE REGEX ".*/Unix/.*")
list(FILTER GENERATED_SOURCES EXCLUDE REGEX ".*/Unix/.*")

target_sources(${PROJECT_NAME} PRIVATE main.cpp ${GENERATED_SOURCES} ${GENERATED_HEADERS})
set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE false)