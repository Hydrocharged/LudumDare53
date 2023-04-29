cmake_minimum_required(VERSION 3.23)
project(LudumDare53)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GALACTIC_ENGINE_ASSETS_PATH "${PROJECT_SOURCE_DIR}/assets")
set(GALACTIC_ENGINE_MATERIALS "${PROJECT_SOURCE_DIR}/materials/*.mat")

# Dependencies
include(FetchContent)
FetchContent_Declare(galacticeengine # April 28, 2023
    GIT_REPOSITORY https://github.com/Hydrocharged/GalacticEngine.git
    GIT_TAG 0658e483af35713bad5da4747dbc2fe254dc6e5a)
FetchContent_Declare(flecs # April 28, 2023
    URL https://github.com/SanderMertens/flecs/archive/665736efce28fdb142f48379875075d02c174bc1.zip)
FetchContent_MakeAvailable(galacticeengine flecs)

# Project
file(GLOB_RECURSE all_SRCS
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/src/*.c")
if(PLATFORM_WIN32)
    add_executable(${PROJECT_NAME} WIN32 ${all_SRCS})
elseif(PLATFORM_MACOS)
    add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${all_SRCS})
else()
    add_executable(${PROJECT_NAME} ${all_SRCS})
endif()
target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(${PROJECT_NAME} PRIVATE GalacticEngine flecs::flecs_static)
galactic_engine_post_build()
