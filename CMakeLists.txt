cmake_minimum_required(VERSION 3.27)
project(enniolimpiadi C)
set(CMAKE_C_STANDARD 99)

# Adding Raylib
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # don't build the supplied example games

FetchContent_Declare(
    raylib
    GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
    GIT_TAG "5.5"
    GIT_PROGRESS TRUE
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(raylib)

add_executable(enniolimpiadi main.c)

target_compile_options(enniolimpiadi PUBLIC)
target_include_directories(enniolimpiadi PUBLIC .)

target_link_libraries(enniolimpiadi PUBLIC raylib)

if(MSVC)
  set(CMAKE_C_FLAGS_DEBUG "/D CLAY_DEBUG")
else()
target_compile_options(enniolimpiadi PRIVATE
  -fcolor-diagnostics
  -fansi-escape-codes
  -std=c99
  -Wall
  # -Werror
  -Wconversion
  -pedantic
)
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}")
endif()

add_custom_command(
  TARGET enniolimpiadi POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/resources
  ${CMAKE_CURRENT_BINARY_DIR}/resources
)
