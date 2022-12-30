cmake_minimum_required(VERSION 3.0)
project(sway CXX)

# Release Build - release by default (except for Borland)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(BASE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src")

set(MAJORVER 0)
set(MINORVER 1)
set(PATCHVER 0)
set(SOLCONSOLE_ENABLED 0)
set(SOLIBVER ${MAJORVER}.${MINORVER}.${PATCHVER})

file(GLOB SOURCES_LIST ${BASE_SOURCE_DIR}/*.cpp)
file(GLOB JSVM_SOURCES_LIST ${BASE_SOURCE_DIR}/jsvm/*.cpp)
file(GLOB UI_SOURCES_LIST ${BASE_SOURCE_DIR}/ui/*.cpp)
file(GLOB MODULES_SOURCES_LIST ${BASE_SOURCE_DIR}/modules/*.cpp)

# AStyle Source
list(APPEND SRCS
    # src/raminfo.cpp
    ${SOURCES_LIST}
    ${JSVM_SOURCES_LIST}
    ${UI_SOURCES_LIST}
    ${MODULES_SOURCES_LIST}
    )

message("CMAKE_BUILD_TYPE is Executable ${CMAKE_BUILD_TYPE}")
set(CMAKE_VERBOSE_MAKEFILE ON)

source_group("jsvm" FILES ${JSVM_SOURCES_LIST})
source_group("ui" FILES ${UI_SOURCES_LIST})
source_group("src" FILES ${SOURCES_LIST})
source_group("modules" FILES ${MODULES_SOURCES_LIST})

include_directories(src)
include_directories(${CMAKE_SOURCE_DIR}/mujs)

get_filename_component(KIT10_DIR "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots;KitsRoot10]" REALPATH)
list(APPEND W10SDK_VERSIONS "10.0.10240.0" "10.0.14393.0" "10.0.15063.0" "10.0.16299.0" "10.0.18362.0")
list(REMOVE_DUPLICATES W10SDK_VERSIONS)
list(SORT W10SDK_VERSIONS)
list(REVERSE W10SDK_VERSIONS)
foreach(W10SDK_VER ${W10SDK_VERSIONS})
    find_library(D3D11_LIBRARY d3d11.lib     PATHS     "$ENV{DXSDK_DIR}/Lib/x64" "C:/Program Files (x86)/Windows Kits/10/lib/${W10SDK_VER}/um/${MSVC_CXX_ARCHITECTURE_ID}/")
    # message( "C:/Program Files (x86)/Windows Kits/10/lib/${W10SDK_VER}/um/${MSVC_CXX_ARCHITECTURE_ID}/")
endforeach()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
    if(${SOLCONSOLE_ENABLED}) 
        add_definitions(-DWIN_SUBSYSTEM_WINDOWS)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
    else()
        add_definitions(-DWIN_SUBSYSTEM_CONSOLE)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE")
    endif()
endif(MSVC)

set(MUJS_HOME "${CMAKE_SOURCE_DIR}/mujs")
add_subdirectory(${MUJS_HOME})

# CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX11)
# set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME "")

add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} ${D3D11_LIBRARY})
if(MSVC)
   target_link_libraries(${PROJECT_NAME} "imagehlp")
   set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
endif(MSVC)

target_link_libraries(${PROJECT_NAME} ${MUJS_LIBRARY})
