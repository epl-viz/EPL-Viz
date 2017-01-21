# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

# This file is based on the FindGTest CMake module (CMake 3.7.1)

#.rst:
# FindGTest
# ---------
#
# Locate the Google C++ Testing Framework.
#
# Imported targets
# ^^^^^^^^^^^^^^^^
#
# This module defines the following :prop_tgt:`IMPORTED` targets:
#
# ``GMock::GMock``
#   The Google GMock ``gtest`` library, if found; adds Thread::Thread
#   automatically
# ``GMock::Main``
#   The Google GMock ``gtest_main`` library, if found
#
#
# Result variables
# ^^^^^^^^^^^^^^^^
#
# This module will set the following variables in your project:
#
# ``GMOCK_FOUND``
#   Found the Google Testing framework
# ``GMOCK_INCLUDE_DIRS``
#   the directory containing the Google Test headers
#
# The library variables below are set as normal variables.  These
# contain debug/optimized keywords when a debugging library is found.
#
# ``GMOCK_LIBRARIES``
#   The Google Test ``gtest`` library; note it also requires linking
#   with an appropriate thread library
# ``GMOCK_MAIN_LIBRARIES``
#   The Google Test ``gtest_main`` library
# ``GMOCK_BOTH_LIBRARIES``
#   Both ``gtest`` and ``gtest_main``
#
# Cache variables
# ^^^^^^^^^^^^^^^
#
# The following cache variables may also be set:
#
# ``GMOCK_ROOT``
#   The root directory of the Google Test installation (may also be
#   set as an environment variable)
# ``GMOCK_MSVC_SEARCH``
#   If compiling with MSVC, this variable can be set to ``MD`` or
#   ``MT`` (the default) to enable searching a GMock build tree
#

function(_gmock_append_debugs _endvar _library)
    if(${_library} AND ${_library}_DEBUG)
        set(_output optimized ${${_library}} debug ${${_library}_DEBUG})
    else()
        set(_output ${${_library}})
    endif()
    set(${_endvar} ${_output} PARENT_SCOPE)
endfunction()

function(_gmock_find_library _name)
    find_library(${_name}
        NAMES ${ARGN}
        HINTS
            ENV GMOCK_ROOT
            ${GMOCK_ROOT}
        PATH_SUFFIXES ${_gmock_libpath_suffixes}
    )
    mark_as_advanced(${_name})
endfunction()

#

if(NOT DEFINED GMOCK_MSVC_SEARCH)
    set(GMOCK_MSVC_SEARCH MD)
endif()

set(_gmock_libpath_suffixes lib)
if(MSVC)
    if(GMOCK_MSVC_SEARCH STREQUAL "MD")
        list(APPEND _gmock_libpath_suffixes
            msvc/gmock-md/Debug
            msvc/gmock-md/Release)
    elseif(GMOCK_MSVC_SEARCH STREQUAL "MT")
        list(APPEND _gmock_libpath_suffixes
            msvc/gmock/Debug
            msvc/gmock/Release)
    endif()
endif()


find_path(GMOCK_INCLUDE_DIR gmock/gmock.h
    HINTS
        $ENV{GMOCK_ROOT}/include
        ${GMOCK_ROOT}/include
)
mark_as_advanced(GMOCK_INCLUDE_DIR)

if(MSVC AND GMOCK_MSVC_SEARCH STREQUAL "MD")
    # The provided /MD project files for Google Test add -md suffixes to the
    # library names.
    _gmock_find_library(GMOCK_LIBRARY            gmock-md  gmock)
    _gmock_find_library(GMOCK_LIBRARY_DEBUG      gmock-mdd gmockd)
    _gmock_find_library(GMOCK_MAIN_LIBRARY       gmock_main-md  gmock_main)
    _gmock_find_library(GMOCK_MAIN_LIBRARY_DEBUG gmock_main-mdd gmock_maind)
else()
    _gmock_find_library(GMOCK_LIBRARY            gmock)
    _gmock_find_library(GMOCK_LIBRARY_DEBUG      gmockd)
    _gmock_find_library(GMOCK_MAIN_LIBRARY       gmock_main)
    _gmock_find_library(GMOCK_MAIN_LIBRARY_DEBUG gmock_maind)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/FindPackageHandleStandardArgs.cmake)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GMock DEFAULT_MSG GMOCK_LIBRARY GMOCK_INCLUDE_DIR GMOCK_MAIN_LIBRARY)

if(GMOCK_FOUND)
    set(GMOCK_INCLUDE_DIRS ${GMOCK_INCLUDE_DIR})
    _gmock_append_debugs(GMOCK_LIBRARIES      GMOCK_LIBRARY)
    _gmock_append_debugs(GMOCK_MAIN_LIBRARIES GMOCK_MAIN_LIBRARY)
    set(GMOCK_BOTH_LIBRARIES ${GMOCK_LIBRARIES} ${GMOCK_MAIN_LIBRARIES})

    include(CMakeFindDependencyMacro)
    find_dependency(Threads)

    if(NOT TARGET GMock::GMock)
        add_library(GMock::GMock UNKNOWN IMPORTED)
        set_target_properties(GMock::GMock PROPERTIES
            INTERFACE_LINK_LIBRARIES "Threads::Threads")
        if(GMOCK_INCLUDE_DIRS)
            set_target_properties(GMock::GMock PROPERTIES
                INTERFACE_INCLUDE_DIRECTORIES "${GMOCK_INCLUDE_DIRS}")
        endif()
        if(EXISTS "${GMOCK_LIBRARY}")
            set_target_properties(GMock::GMock PROPERTIES
                IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
                IMPORTED_LOCATION "${GMOCK_LIBRARY}")
        endif()
        if(EXISTS "${GMOCK_LIBRARY_RELEASE}")
            set_property(TARGET GMock::GMock APPEND PROPERTY
                IMPORTED_CONFIGURATIONS RELEASE)
            set_target_properties(GMock::GMock PROPERTIES
                IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
                IMPORTED_LOCATION_RELEASE "${GMOCK_LIBRARY_RELEASE}")
        endif()
        if(EXISTS "${GMOCK_LIBRARY_DEBUG}")
            set_property(TARGET GMock::GMock APPEND PROPERTY
                IMPORTED_CONFIGURATIONS DEBUG)
            set_target_properties(GMock::GMock PROPERTIES
                IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
                IMPORTED_LOCATION_DEBUG "${GMOCK_LIBRARY_DEBUG}")
        endif()
      endif()
      if(NOT TARGET GMock::Main)
          add_library(GMock::Main UNKNOWN IMPORTED)
          set_target_properties(GMock::Main PROPERTIES
              INTERFACE_LINK_LIBRARIES "GMock::GMock")
          if(EXISTS "${GMOCK_MAIN_LIBRARY}")
              set_target_properties(GMock::Main PROPERTIES
                  IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
                  IMPORTED_LOCATION "${GMOCK_MAIN_LIBRARY}")
          endif()
          if(EXISTS "${GMOCK_MAIN_LIBRARY_RELEASE}")
            set_property(TARGET GMock::Main APPEND PROPERTY
                IMPORTED_CONFIGURATIONS RELEASE)
            set_target_properties(GMock::Main PROPERTIES
                IMPORTED_LINK_INTERFACE_LANGUAGES_RELEASE "CXX"
                IMPORTED_LOCATION_RELEASE "${GMOCK_MAIN_LIBRARY_RELEASE}")
          endif()
          if(EXISTS "${GMOCK_MAIN_LIBRARY_DEBUG}")
            set_property(TARGET GMock::Main APPEND PROPERTY
                IMPORTED_CONFIGURATIONS DEBUG)
            set_target_properties(GMock::Main PROPERTIES
                IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
                IMPORTED_LOCATION_DEBUG "${GMOCK_MAIN_LIBRARY_DEBUG}")
          endif()
    endif()
endif()
