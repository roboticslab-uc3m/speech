# Find the Vosk library and headers.
#
# Sets the following variables:
#
# VOSK_FOUND        - system has Vosk
# VOSK_INCLUDE_DIRS - Vosk include directories
# VOSK_LIBRARIES    - Vosk libraries
#
# ...and the following imported targets (requires CMake 2.8.11+):
#
# Vosk::Vosk        - Vosk library

find_path(VOSK_INCLUDE_DIR NAMES vosk_api.h)
find_library(VOSK_LIBRARY NAMES vosk libvosk)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Vosk DEFAULT_MSG VOSK_INCLUDE_DIR VOSK_LIBRARY)

if(VOSK_FOUND)
    set(VOSK_INCLUDE_DIRS ${VOSK_INCLUDE_DIR})
    set(VOSK_LIBRARIES ${VOSK_LIBRARY})

    if(NOT TARGET Vosk::Vosk)
        add_library(Vosk::Vosk UNKNOWN IMPORTED)

        set_target_properties(Vosk::Vosk PROPERTIES IMPORTED_LOCATION "${VOSK_LIBRARY}"
                                                    INTERFACE_INCLUDE_DIRECTORIES "${VOSK_INCLUDE_DIR}")
    endif()
endif()

mark_as_advanced(VOSK_INCLUDE_DIR VOSK_LIBRARY)
