# https://stackoverflow.com/a/25217937
get_directory_property(_has_parent PARENT_DIRECTORY)

if(NOT _has_parent)
    cmake_minimum_required(VERSION 3.9)
    project(doxygen NONE)
    find_package(Doxygen REQUIRED)
elseif(NOT DOXYGEN_FOUND)
    return()
endif()

set(DOXYGEN_PROJECT_NAME "speech")
set(DOXYGEN_REPEAT_BRIEF NO)
set(DOXYGEN_EXTRACT_PRIVATE YES)
set(DOXYGEN_EXTRACT_STATIC YES)
set(DOXYGEN_EXAMPLE_PATH ./examples)
set(DOXYGEN_EXAMPLE_RECURSIVE YES)
set(DOXYGEN_IMAGE_PATH ./doc/fig)
set(DOXYGEN_HTML_TIMESTAMP YES)
set(DOXYGEN_USE_MATHJAX YES)
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE ./README.md)

set(_doxygen_input README.md
                   doc
                   examples
                   libraries
                   programs
                   share)

doxygen_add_docs(dox ${_doxygen_input}
                 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/..)
