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
set(SOLIBVER ${MAJORVER}.${MINORVER}.${PATCHVER})

# AStyle Source
list(APPEND SRCS
    src/main.cpp
    src/imgui.cpp
    src/imgui_impl_dx11.cpp
    src/imgui_impl_win32.cpp
    src/imgui_tables.cpp
    src/imgui_widgets.cpp
    src/imgui_draw.cpp)

message("CMAKE_BUILD_TYPE is Executable ${CMAKE_BUILD_TYPE}")
set(CMAKE_VERBOSE_MAKEFILE ON)

get_filename_component(KIT10_DIR "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows Kits\\Installed Roots;KitsRoot10]" REALPATH)
list(APPEND W10SDK_VERSIONS "10.0.10240.0" "10.0.14393.0" "10.0.15063.0" "10.0.16299.0" "10.0.18362.0")
list(REMOVE_DUPLICATES W10SDK_VERSIONS)
list(SORT W10SDK_VERSIONS)
list(REVERSE W10SDK_VERSIONS)
foreach(W10SDK_VER ${W10SDK_VERSIONS})
    find_library(D3D11_LIBRARY d3d11.lib     PATHS     "$ENV{DXSDK_DIR}/Lib/x64" "C:/Program Files (x86)/Windows Kits/10/lib/${W10SDK_VER}/um/${MSVC_CXX_ARCHITECTURE_ID}/")
    # message( "C:/Program Files (x86)/Windows Kits/10/lib/${W10SDK_VER}/um/${MSVC_CXX_ARCHITECTURE_ID}/")
endforeach()
# Enforce C++11
set(USE_CPP11 ON)
# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
# CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
# set_property(TARGET ${PROJECT_NAME} PROPERTY OUTPUT_NAME "")

add_executable(${PROJECT_NAME} ${SRCS})
target_link_libraries(${PROJECT_NAME} ${D3D11_LIBRARY})