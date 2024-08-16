# Find the Espeak libraries and headers.
#
# Sets the following variables:
#
# ESPEAK_FOUND        - system has Espeak
# ESPEAK_INCLUDE_DIRS - Espeak include directories
# ESPEAK_LIBRARIES    - Espeak libraries
#
# ...and the following imported targets (requires CMake 2.8.11+):
#
# Espeak::Espeak      - Espeak library (old API)

find_path(ESPEAK_INCLUDE_DIR NAMES speak_lib.h
                             HINTS ENV ESPEAK_DIR
                             PATH_SUFFIXES espeak)

find_library(ESPEAK_LIBRARY NAMES espeak libespeak)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Espeak DEFAULT_MSG ESPEAK_INCLUDE_DIR ESPEAK_LIBRARY)

if(Espeak_FOUND)
    set(ESPEAK_INCLUDE_DIRS ${ESPEAK_INCLUDE_DIR})
    set(ESPEAK_LIBRARIES ${ESPEAK_LIBRARY})

    if(NOT TARGET Espeak::Espeak)
        add_library(Espeak::Espeak UNKNOWN IMPORTED)

        set_target_properties(Espeak::Espeak PROPERTIES IMPORTED_LOCATION "${ESPEAK_LIBRARY}"
                                                        INTERFACE_INCLUDE_DIRECTORIES "${ESPEAK_INCLUDE_DIR}")
    endif()
endif()

mark_as_advanced(ESPEAK_INCLUDE_DIR ESPEAK_LIBRARY)
