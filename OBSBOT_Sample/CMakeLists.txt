project(OBSBOT_Sample)

# Prohibit in-source builds
message(STATUS "CMAKE_BINARY_DIR = ${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_SOURCE_DIR = ${CMAKE_SOURCE_DIR}")
if ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
    message(FATAL_ERROR "RM: You cannot build in a source directory (or any directory with CMakeLists.txt file). Please"
            "make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif ()

set(CMAKE_CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED true)

set(OBSBOT_Target "OBSBOT_Sample")

add_executable(${OBSBOT_Target} main.cpp)


if (CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    set(SDK_LINK_PATH ${PROJECT_SOURCE_DIR}/../linux/x86_64-release)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
    set(SDK_LINK_PATH ${PROJECT_SOURCE_DIR}/../linux/arm64-release)
else()
    message(FATAL_ERROR "Unsupported architecture: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

target_sources(${OBSBOT_Target} PRIVATE main.cpp)

include_directories(${CMAKE_SOURCE_DIR}/../include)

target_link_directories(${OBSBOT_Target} PRIVATE "${SDK_LINK_PATH}")
target_link_libraries(${OBSBOT_Target} PRIVATE dev)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations -Wno-switch")


# install
set(PROJECT_RUN_DIR ${CMAKE_BINARY_DIR}/rundir)

# target
install(TARGETS ${OBSBOT_Target} DESTINATION ${PROJECT_RUN_DIR})
# depend lib
install(DIRECTORY ${SDK_LINK_PATH}/ DESTINATION ${PROJECT_RUN_DIR})

