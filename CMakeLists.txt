# Copyright (c) 2017, EPL-Vizards
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the EPL-Vizards nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL EPL-Vizards BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

cmake_minimum_required( VERSION 3.5 )
project( EPL-Viz )

message( STATUS "Using CMake version: ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}\n" )
include( ${PROJECT_SOURCE_DIR}/cmake/cmakeScripLoader.cmake )

include( CMakePackageConfigHelpers )
include( ExternalProject )

######################
# Set some variables #
######################

# Set the binary output directories
set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin )

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake )

set( CMAKE_CXX_STANDARD          14 )
set( CMAKE_CXX_STANDARD_REQUIRED ON )
set( CMAKE_C_STANDARD            11 )
set( CMAKE_C_STANDARD_REQUIRED   ON )

set( CMAKE_INCLUDE_CURRENT_DIR       ON )
set( CMAKE_POSITION_INDEPENDENT_CODE ON )

option( ENABLE_CODE_COVERAGE "Enables code coverage tests"                   OFF )
option( USE_SYSTEM_PACKETS   "Use the EPL DataCollect version of the system" ON )
option( USE_KTEXTEDITOR      "Use the KF5 Texteditor"                        ON )

list( APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" )

# Some useful variables
set( EPL_VIZ_TEMPLATES_DIR "${PROJECT_SOURCE_DIR}/cmake/templates" )
set( EPL_VIZ_INCLUDES_DIR  "${PROJECT_SOURCE_DIR}/include" )
set( PLF_COLONY_DIR        "${PROJECT_SOURCE_DIR}/external/EPL_DataCollect/external/plf_colony/" )

##############################################
# Tell the great Windows compiler what to do #
##############################################

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(WIN32)
  set(WIRESHARK_TARGET_PLATFORM win64)
  set(PROCESSOR_ARCHITECTURE amd64)
endif(WIN32)

if( WIRESHARK_BASE_DIR )
  set( ENV{WIRESHARK_BASE_DIR} "${WIRESHARK_BASE_DIR}" )
  set( ENV{WIRESHARK_LIB_DIR}  "${WIRESHARK_BASE_DIR}/wireshark-${WIRESHARK_TARGET_PLATFORM}-libs-2.4" )
  if( NOT EXISTS "$ENV{WIRESHARK_LIB_DIR}" )
    set( ENV{WIRESHARK_LIB_DIR}  "${WIRESHARK_BASE_DIR}/wireshark-${WIRESHARK_TARGET_PLATFORM}-libs" )
  endif( NOT EXISTS "$ENV{WIRESHARK_LIB_DIR}" )
endif( WIRESHARK_BASE_DIR )

################################
# Set up the build environment #
################################

if( ENABLE_CODE_COVERAGE )
  set( COVERAGE_COMPILER_FLAG "--coverage" )
endif( ENABLE_CODE_COVERAGE )

run_git()

add_compiler(
  GNU MIN_VERSION "6.2"
  ALL        "-Wall -Wextra -Wfloat-equal -Wshadow -Wno-unknown-pragmas -fno-strict-aliasing -fdiagnostics-color=always"
  DEBUG      "-O0 -Werror" ${COVERAGE_COMPILER_FLAG}
  RELEASE    "-O3"
  SANITIZE   "-fsanitize=${SANITIZERS}"
)

add_compiler(
  Clang MIN_VERSION 3.9
  ALL        "-Weverything -ftemplate-backtrace-limit=0 -fno-strict-aliasing -fcolor-diagnostics"
             "-Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded"
             "-Wno-gnu-anonymous-struct -Wno-nested-anon-types -Wno-exit-time-destructors"
             "-Wno-global-constructors -Wno-switch-enum -Wno-reserved-id-macro"
             "-Wno-documentation-unknown-command"
             "-Wno-undefined-reinterpret-cast" # For some reason QMOC produces code generating this warning
  DEBUG      "-Werror"
  RELEASE    "-O3"
  SANITIZE "-fsanitize=${SANITIZERS}"
)

add_definitions(-DWS_NORETURN=) # For some reason wireshark needs this

######################
# Check Dependencies #
######################

set( EPL_VIZ_QT_COMPONENTS Core Widgets )

find_package( ECM 1.1.0 REQUIRED NO_MODULE )
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} )

find_package( Qt5 5.5 COMPONENTS ${EPL_VIZ_QT_COMPONENTS} REQUIRED )
find_package( Qwt              REQUIRED )
find_package( PythonInterp 3.5 REQUIRED ) # Exec python with PYTHON_EXECUTABLE
find_package( PythonLibs   3.5 REQUIRED )
find_package( Wireshark        REQUIRED )
find_package( PCAP             REQUIRED )
find_package( PkgConfig        REQUIRED )
find_package( Threads          REQUIRED )
find_package( TinyXML2     4.0 REQUIRED )
pkg_search_module(GLIB REQUIRED glib-2.0)

if( USE_KTEXTEDITOR )
  find_package( KF5     REQUIRED COMPONENTS TextEditor)
  set( KDE_LIBRARIES KF5::TextEditor )
  set( PLATFORM_TARGET "ALL_KDE" )
else( USE_KTEXTEDITOR )
  set( KDE_LIBRARIES "" )
  set( PLATFORM_TARGET "" )
endif( USE_KTEXTEDITOR )


set( CMAKE_AUTOUIC OFF )

link_directories( ${Wireshark_LIB_DIR} )

# Generate link target list
set( EPL_VIZ_QT_DEP_LIST "" )
foreach( I IN LISTS EPL_VIZ_QT_COMPONENTS )

  # Fix Qt compiler warnings (mark qt include dirs as system)
  if( CMAKE_CXX_COMPILER_ID STREQUAL Clang OR CMAKE_CXX_COMPILER_ID STREQUAL GNU )
    foreach( J IN LISTS Qt5${I}_INCLUDE_DIRS )
      if( NOT "${J}" MATCHES "[qQ][tT]" )
        continue()
      endif( NOT "${J}" MATCHES "[qQ][tT]" )
      string( APPEND CMAKE_CXX_FLAGS " -isystem ${J}" )
    endforeach( J IN LISTS Qt5${I}_INCLUDE_DIRS )
  endif( CMAKE_CXX_COMPILER_ID STREQUAL Clang OR CMAKE_CXX_COMPILER_ID STREQUAL GNU  )

  list( APPEND EPL_VIZ_QT_DEP_LIST "Qt5::${I}" )
endforeach( I IN LISTS EPL_VIZ_QT_COMPONENTS )


find_package( Git REQUIRED )


#############################
# Add internal dependencies #
#############################

if( USE_SYSTEM_PACKETS )

  # Use the EPL_DataCollect version installed on the system
  find_package( EPL_DataCollect REQUIRED )

else( USE_SYSTEM_PACKETS )
  set( EPL_DC_PREFIX         "${PROJECT_BINARY_DIR}/EPL_DataCollect" )
  set( EPL_DC_BUILD_DIR      "${EPL_DC_PREFIX}/build" )
  set( EPL_DC_INSTALL_DIR    "${EPL_DC_PREFIX}/install_dir" )
  set( EPL_DC_LIB_DIR        "${EPL_DC_INSTALL_DIR}/lib/eplViz" )
  set( EPL_DC_LIB_WS_LIB_DIR "${EPL_DC_INSTALL_DIR}/lib" )

  set( EPL_DataCollect_INCLUDE_DIRS "${EPL_DC_INSTALL_DIR}/include/eplViz" )

  macro( importLibs )
    foreach( I IN ITEMS ${ARGV} )
      set( LIB_DIR "${EPL_DC_LIB_DIR}" )
      set( SHARED_SUFFIX "_shared" )

      if( NOT "${I}" MATCHES "epl.?[a-zA-Z0-9]+" )
        set( LIB_DIR "${EPL_DC_LIB_WS_LIB_DIR}" )
        set( SHARED_SUFFIX "" )
      endif( NOT "${I}" MATCHES "epl.?[a-zA-Z0-9]+" )

      string( APPEND EPL_BYPRODUCT_STR "${LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}${I}${CMAKE_SHARED_LIBRARY_SUFFIX}" )
      string( APPEND EPL_BYPRODUCT_STR "${LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${I}${CMAKE_STATIC_LIBRARY_SUFFIX}" )
      add_library( ${I}${SHARED_SUFFIX} SHARED IMPORTED )
      add_library( ${I}_static          STATIC IMPORTED )
      set_target_properties( ${I}${SHARED_SUFFIX} PROPERTIES IMPORTED_LOCATION "${LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}${I}${CMAKE_SHARED_LIBRARY_SUFFIX}" )
      set_target_properties( ${I}_static          PROPERTIES IMPORTED_LOCATION "${LIB_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}${I}${CMAKE_STATIC_LIBRARY_SUFFIX}" )
    endforeach( I IN ITEMS ${ARGV} )
  endmacro( importLibs )

  importLibs( epl_DataCollect eplp_timeSeries eplp_protocolValidator eplp_python eplp_defaultFilter libwireshark )

  ExternalProject_Add( EPL_DataCollect
    SOURCE_DIR        "${PROJECT_SOURCE_DIR}/external/EPL_DataCollect"
    BINARY_DIR        "${EPL_DC_BUILD_DIR}"
    PREFIX            "${EPL_DC_PREFIX}"
    CMAKE_ARGS
      "-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}"
      "-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}"
      "-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}"
      "-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}"
      "-DCMAKE_INSTALL_PREFIX=${EPL_DC_INSTALL_DIR}"
      -DDISABLE_TESTS=ON
      -DENABLE_CODE_COVERAGE=OFF
      -DCMAKE_INSTALL_MESSAGE=NEVER
      "-DWireshark_DIR=${Wireshark_DIR}"
      "-DPCAP_HINTS=${PCAP_HINTS}"
      "-DTinyXML2_ROOT=${TinyXML2_ROOT}"
    BUILD_BYPRODUCTS ${EPL_BYPRODUCT_STR}
  )
endif( USE_SYSTEM_PACKETS )

####################
# Generate subdirs #
####################

# Write dummy files, so that they can be detected
file( WRITE "${PROJECT_SOURCE_DIR}/libEPLViz/src/utils/EPLVizEnum2Str.hpp" "" )
file( WRITE "${PROJECT_SOURCE_DIR}/libEPLViz/src/utils/EPLVizEnum2Str.cpp" "" )

message( "" )

add_platform( TARGET KDE )
check_platform()

new_project_library(
  PATH         "${PROJECT_SOURCE_DIR}/libEPLViz"
  NAME         "libEPLViz"
  TEMPLATE     "${EPL_VIZ_TEMPLATES_DIR}/CMakeLists.lib.txt"
  DEPENDENCIES ${EPL_VIZ_QT_DEP_LIST}
    Qwt::Qwt ${KDE_LIBRARIES} epl_DataCollect_shared
    eplp_timeSeries_shared eplp_python_shared eplp_protocolValidator_shared eplp_defaultFilter_shared
)

new_project_executable(
  PATH         "${PROJECT_SOURCE_DIR}/epl-viz"
  NAME         "eplviz"
  TEMPLATE     "${EPL_VIZ_TEMPLATES_DIR}/CMakeLists.exe.txt"
  DEPENDENCIES libEPLViz
)

##############################
# Update include directories #
##############################

include_directories(
  ${EPL-Viz_LIB_INCLUDE_DIRECTORIES}
  ${EPL_VIZ_INCLUDES_DIR}
)

include_directories( SYSTEM
  ${CATCH_INCLUDE_DIR}
  ${FAKE_IT_INCLUDE_DIR}
  ${EPL_DataCollect_INCLUDE_DIRS}
  ${Wireshark_INCLUDE_DIR}
  ${PYTHON_INCLUDE_DIRS}
  ${GLIB_INCLUDE_DIRS}
  ${QWT_INCLUDE_DIRS}
  ${PLF_COLONY_DIR}
  ${PCAP_INCLUDE_DIRS}
)

link_directories( ${Wireshark_LIB_DIR} ${EPL_DC_LIB_WS_LIB_DIR} )

# Add Qt5 include dirs
foreach( I IN LISTS EPL_VIZ_QT_COMPONENTS )
  include_directories( SYSTEM ${Qt5${I}_INCLUDE_DIRS} )
endforeach( I IN LISTS EPL_VIZ_QT_COMPONENTS )

if( NOT EXISTS "${EPL_VIZ_INCLUDES_DIR}" )
  file( MAKE_DIRECTORY "${EPL_VIZ_INCLUDES_DIR}" )
endif( NOT EXISTS "${EPL_VIZ_INCLUDES_DIR}" )

######################
# Add subdirectories #
######################

foreach( I IN LISTS EPL-Viz_SUBDIR_LIST )
  add_subdirectory( "${I}" )
endforeach( I IN LISTS EPL-Viz_SUBDIR_LIST )

if( NOT USE_SYSTEM_PACKETS )
  add_dependencies( libEPLViz EPL_DataCollect )
endif( NOT USE_SYSTEM_PACKETS )

##################
# Generate Files #
##################

enum2str_generate(
  PATH       "${PROJECT_SOURCE_DIR}/libEPLViz/src/utils"
  CLASS_NAME "EPLVizEnum2Str"
  FUNC_NAME  "toStr"
  INCLUDES   "GUIState.hpp" "PacketVizModel.hpp"
  INDENT_STR "  "
  NAMESPACE  "EPL_Viz"
  ENUMS
    "GUIState" "PacketVizModel::CycleTimeing"
  BLACKLIST
)

configure_file( "${EPL_VIZ_TEMPLATES_DIR}/defines.in.hpp" "${EPL_VIZ_INCLUDES_DIR}/EPLVizDefines.hpp" @ONLY )

if( USE_SYSTEM_PACKETS )
  write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Version.cmake
      VERSION       ${CM_VERSION_MAJOR}.${CM_VERSION_MINOR}.${CM_VERSION_SUBMINOR}
      COMPATIBILITY SameMajorVersion
  )

  install(
    FILES
      ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Version.cmake
      ${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake
    DESTINATION lib/cmake/${PROJECT_NAME}
  )

  install( EXPORT ${PROJECT_NAME}Targets DESTINATION lib/cmake/${PROJECT_NAME} )
endif( USE_SYSTEM_PACKETS )

install(
  FILES "${EPL_VIZ_INCLUDES_DIR}/EPLVizDefines.hpp"
  DESTINATION include/eplViz
)

generate_format_command( format 3.9.1 )
