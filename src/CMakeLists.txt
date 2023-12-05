cmake_minimum_required(VERSION 3.22)

# Add subdirectories
add_subdirectory(bison-flex)

# Add the binary and sources
add_executable(
  C3MS
  main.cc
)

target_link_libraries(C3MS c3ms)
set_target_properties(C3MS PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../..")
