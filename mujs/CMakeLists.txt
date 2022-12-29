project(mujs C)

file(GLOB MUJS_HEADERS_LIST "*.h")
file(GLOB MUJS_SOURCES_LIST "*.c")

add_library(${PROJECT_NAME} STATIC ${MUJS_SOURCES_LIST})

source_group("mujs" FILES ${MUSJ_SOURCES_LIST})

# Required on Unix OS family to be able to be linked into shared libraries.
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_source_files_properties(${MUSJ_SOURCES_LIST} PROPERTIES LANGUAGE C)

if(NOT CMAKE_BUILD_TYPE_RELEASE AND NOT MSVC)
  message ("mujs built in debug mode")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
endif()


# Expose public includes to other through cache variable.
set(MUJS_INCLUDE_DIR ${PROJECT_SOURCE_DIR}
    CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE)

set(MUJS_LIBRARY ${PROJECT_NAME}
    CACHE INTERNAL "${PROJECT_NAME}: Library Name" FORCE)
