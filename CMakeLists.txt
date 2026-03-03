cmake_minimum_required(VERSION 3.13)
project(bwparser)

set(SOURCE_FILES bwparser.c)

# Grok-generated condition setup for Windows version of argp
if(WIN32)
  add_subdirectory(thirdparty/argp-standalone)
  add_executable(bwparser ${SOURCE_FILES})

  # IDK what these do exactly
  target_link_libraries(bwparser PRIVATE argp-standalone)
  target_include_directories(bwparser PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/argp-standalone/include/argp-standalone)
else()
  add_executable(bwparser ${SOURCE_FILES})
endif()
