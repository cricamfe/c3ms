cmake_minimum_required(VERSION 3.22)

project(c3ms)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

file(GLOB HH_FILES *.hh)
file(GLOB SRC_FILES *.cc)
file(GLOB HXX_FILES *.hxx)

find_package(BISON)
find_package(FLEX)

bison_target(CodeParser parse.yy ${CMAKE_CURRENT_BINARY_DIR}/parser.cc)
flex_target(CodeScanner scan.ll ${CMAKE_CURRENT_BINARY_DIR}/scanner.cc)

add_flex_bison_dependency(CodeScanner CodeParser)

add_library(
            c3ms
            STATIC
            ${FLEX_CodeScanner_OUTPUTS}
            ${BISON_CodeParser_OUTPUTS}
            ${HH_FILES}
            ${SRC_FILES}
            ${HXX_FILES}
)
