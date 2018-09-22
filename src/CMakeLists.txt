include(ProcessorCount)
ProcessorCount(CORES)

set(CMAKE_CXX_STANDARD 17)

# Needed tools

find_program(MAKE make)
if(NOT MAKE)
  message(FATAL_ERROR "make is needed but not found (On ubuntu: sudo apt install make)")
endif(NOT MAKE)

find_program(AUTOCONF autoconf)
if(NOT AUTOCONF)
  message(FATAL_ERROR "autoconf is needed but not found (On ubuntu: sudo apt install autoconf)")
endif(NOT AUTOCONF)

find_program(LIBTOOL libtool)
if(NOT LIBTOOL)
  message(FATAL_ERROR "libtool is needed but not found (On ubuntu: sudo apt install libtool-bin)")
endif(NOT LIBTOOL)

find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)

set(KYRA_DIR ${CMAKE_SOURCE_DIR}/kyra2_1_3)
set(KRENCODER ${KYRA_DIR}/Kyra/encoder/krencoder)

add_custom_command(
  OUTPUT ${KRENCODER}
  COMMAND ./autogen && ${MAKE} -j${CORES}
  WORKING_DIRECTORY ${KYRA_DIR}/Kyra
  )

add_custom_command(
  OUTPUT ${CMAKE_SOURCE_DIR}/images/bb.h
  COMMAND ${KRENCODER} -pBB_ ${CMAKE_SOURCE_DIR}/images/bb.h bricks.xml
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/images
  DEPENDS ${KRENCODER} ${CMAKE_SOURCE_DIR}/images/bricks.xml
  )

add_custom_command(
  OUTPUT ${CMAKE_SOURCE_DIR}/images/bb_bg.h
  COMMAND ${KRENCODER} -pBB_BG_ ${CMAKE_SOURCE_DIR}/images/bb_bg.h bg.xml
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/images
  DEPENDS ${KRENCODER} ${CMAKE_SOURCE_DIR}/images/bg.xml
  )

add_compile_options(-Wall -Werror)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  add_compile_options(-Winconsistent-missing-override)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  add_compile_options(-Wsuggest-override)
endif()

find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)
if (CMAKE_CXX_CPPCHECK)
  list(
    APPEND CMAKE_CXX_CPPCHECK
    "--enable=all"
    "--force"
    "--quiet"
    "--inline-suppr"
    "--error-exitcode=1"
    "--suppressions-list=${CMAKE_SOURCE_DIR}/.cppcheck.sup"
     )
else()
  message(WARNING "cppcheck not found - analysis will not run")
  set(CMAKE_CXX_CPPCHECK "")
endif()

message(STATUS "cppcheck:${CMAKE_CXX_CPPCHECK}")

add_executable(brainblast
  ${CMAKE_CURRENT_SOURCE_DIR}/BrainAI.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/BrainPlayer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/BrainPlayerManager.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/BrainSound.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/BrainSoundFMODEX.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/BrainSprite.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/BrainState.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/BrainText.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Brainblast.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Brick.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Field.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/HighScore.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/Puzzle.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/TextListener.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/bbc.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/consolefont.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_SOURCE_DIR}/images/bb.h
  ${CMAKE_SOURCE_DIR}/images/bb_bg.h

  # Alternate sound implementations
  #${CMAKE_CURRENT_SOURCE_DIR}/BrainSoundBASS.cpp
  #${CMAKE_CURRENT_SOURCE_DIR}/BrainSoundFMOD.cpp
  #${CMAKE_CURRENT_SOURCE_DIR}/BrainSoundSDL.cpp
  )

include_directories(
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src
  ${KYRA_DIR}
  ${SDL_IMAGE_INCLUDE_DIRS}
  )

target_link_libraries(brainblast
  ${CMAKE_SOURCE_DIR}/fmod/fmodex/api/lib/libfmodex64.so
  ${KYRA_DIR}/Kyra/engine/.libs/libengine.a
  ${KYRA_DIR}/Kyra/.libs/libkyra.a
  ${SDL_IMAGE_LIBRARIES}
  ${SDL_LIBRARY}
  )
