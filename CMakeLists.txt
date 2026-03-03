cmake_minimum_required(VERSION 3.13)
project(cookworm-c)

set(SOURCE_FILES bwparser.c)

# Grok-generated condition setup for Windows version of argp
if(WIN32)
  add_subdirectory(thirdparty/argp-standalone)
  add_executable(cookworm-c ${SOURCE_FILES})

  # IDK what these do exactly
  target_link_libraries(cookworm-c PRIVATE argp-standalone)
  target_include_directories(cookworm-c PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/argp-standalone/include/argp-standalone)
else()
  add_executable(cookworm-c ${SOURCE_FILES})
endif()
