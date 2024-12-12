set(APP_NAME "mzgl-livecode")

cmake_minimum_required(VERSION 3.18)

project(${APP_NAME} VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)

# must be in debug not to strip symbols
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(MZGL_ROOT "../koala/lib/mzgl")
get_filename_component(MZGL_ROOT_ABSOLUTE "${MZGL_ROOT}/lib" ABSOLUTE "${CMAKE_CURRENT_SOURCE_DIR}")
add_definitions(-DMZGL_LIBROOT="${MZGL_ROOT_ABSOLUTE}")
include_directories(src)

include(${MZGL_ROOT}/cmake/mzgl.cmake)
set(SOURCES src/main.cpp)

if (WIN32)
    add_executable(${APP_NAME} WIN32 ${SOURCES})
else ()
    add_executable(${APP_NAME} ${SOURCES})
endif ()

add_subdirectory(${MZGL_ROOT} "mzgl-build")

add_compile_definitions(GL_SILENCE_DEPRECATION MZGL_GL2 MZGL_MAC_GLFW __MACOSX_CORE__)

set_target_properties(${APP_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin
)

target_link_options(${APP_NAME} PRIVATE -Wl,-all_load)
add_dependencies(${APP_NAME} libmzgl)
target_link_libraries(${APP_NAME} libmzgl)
