cmake_minimum_required(VERSION 3.14)
project(voxelizer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# ---------------------------------------------------
# Compilation flags
# ---------------------------------------------------
set(STRICS_WARNINGS "-Wall" "-Wextra" "-Wpedantic" "-Werror")
set(SANITIZE_FLAGS "-fsanitize=address,undefined" "-g" "-fno-omit-frame-pointer")

# source files for the unit tests
set(TESTSOURCES 
    tests/test_chunk.cpp
    tests/test_chunkMesher.cpp
    tests/test_perlinNoise.cpp
    )
# source files for the core library without the main function
set(CORE_SOURCES
    src/chunk.hpp
    src/chunkMesher.hpp
    src/perlinNoise.hpp
    src/perlinNoise.cpp
    src/shader.hpp
    src/utils.hpp
    src/world.hpp
    src/worldGenerator.hpp
    src/worldGenerator.cpp
)

# ---------------------------------------------------
# Gtest setup
# ---------------------------------------------------
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# ---------------------------------------------------
# Dependencies
# ---------------------------------------------------
find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

# ---------------------------------------------------
# 3rd party libraries
# ---------------------------------------------------
add_library(glad_lib STATIC extern/glad/src/glad.c)
target_include_directories(glad_lib PUBLIC 
    ${CMAKE_SOURCE_DIR}/extern
)

# ---------------------------------------------------
# Main application executable
# ---------------------------------------------------
add_executable(voxelizer 
    src/main.cpp 
    ${CORE_SOURCES} 
)

target_compile_options(voxelizer PRIVATE ${CMAKE_CXX_FLAGS} ${STRICS_WARNINGS} ${SANITIZE_FLAGS})
target_link_options(voxelizer PRIVATE ${SANITIZE_FLAGS})

target_link_libraries(voxelizer PRIVATE 
    glfw 
    GL 
    m
    Threads::Threads 
    glad_lib 
    )

target_include_directories(voxelizer PRIVATE 
    ${CMAKE_SOURCE_DIR}/extern
    #${CMAKE_SOURCE_DIR}/extern/KHR
    )

# ---------------------------------------------------
# Unit tests executable
# ---------------------------------------------------
enable_testing()
add_executable(unit_tests ${TESTSOURCES} ${CORE_SOURCES})

target_compile_options(unit_tests PRIVATE ${CMAKE_CXX_FLAGS} ${STRICS_WARNINGS} ${SANITIZE_FLAGS})
target_link_options(unit_tests PRIVATE ${SANITIZE_FLAGS})

target_link_libraries(unit_tests PRIVATE 
    GTest::gtest_main 
    Threads::Threads 
    m
)

target_include_directories(unit_tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/extern
)

# Discover and register individual unit tests
include(GoogleTest)
gtest_discover_tests(unit_tests)