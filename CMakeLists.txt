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

# Some useful variables
set( EPL_VIZ_TEMPLATES_DIR ${PROJECT_SOURCE_DIR}/cmake/templates )

################################
# Set up the build environment #
################################

run_git()

add_compiler(
  GNU MIN_VERSION "6.2"
  ALL        "-Wall -Wextra -Wno-comment"
  DEBUG      "-Werror"
  RELEASE    "-O3"
  SANITIZE   "-fsanitize=${SANITIZERS}"
)

add_compiler(
  Clang MIN_VERSION 3.9
  ALL        "-Weverything -ftemplate-backtrace-limit=0"
             "-Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-padded -Wno-float-equal"
             "-Wno-gnu-anonymous-struct -Wno-nested-anon-types -Wno-exit-time-destructors"
             "-Wno-global-constructors -Wno-switch-enum -Wno-reserved-id-macro"
             "-Wno-documentation-unknown-command"
  DEBUG      "-Werror"
  RELEASE    "-O2"
  SANITIZE "-fsanitize=${SANITIZERS}"
)

######################
# Check Dependencies #
######################

set( EPL_VIZ_QT_COMPONENTS Core Widgets )

find_package( Qt5 5.7 COMPONENTS ${EPL_VIZ_QT_COMPONENTS} REQUIRED )

####################
# Generate subdirs #
####################

# Generate link target list
set( EPL_VIZ_QT_DEP_LIST "" )
foreach( I IN LISTS EPL_VIZ_QT_COMPONENTS )

  # Fix Qt compiler warnings (mark qt include dirs as system)
  if( CMAKE_CXX_COMPILER_ID STREQUAL Clang )
    foreach( J IN LISTS Qt5${I}_INCLUDE_DIRS )
      if( NOT "${J}" MATCHES "[qQ][tT]" )
        continue()
      endif( NOT "${J}" MATCHES "[qQ][tT]" )
      string( APPEND CMAKE_CXX_FLAGS " -isystem ${J}" )
    endforeach( J IN LISTS Qt5${I}_INCLUDE_DIRS )
  endif( CMAKE_CXX_COMPILER_ID STREQUAL Clang )

  list( APPEND EPL_VIZ_QT_DEP_LIST "Qt5::${I}" )
endforeach( I IN LISTS EPL_VIZ_QT_COMPONENTS )

new_project_library(
  PATH         "${PROJECT_SOURCE_DIR}/libEPLViz"
  NAME         "libEPLViz"
  TEMPLATE     "${EPL_VIZ_TEMPLATES_DIR}/CMakeLists.lib.txt"
  DEPENDENCIES ${EPL_VIZ_QT_DEP_LIST}
)

new_project_executable(
  PATH         "${PROJECT_SOURCE_DIR}/epl-viz"
  NAME         "EPLViz"
  TEMPLATE     "${EPL_VIZ_TEMPLATES_DIR}/CMakeLists.exe.txt"
  DEPENDENCIES libEPLViz_static
)


##################
# Generate Files #
##################

include_directories( ${EPL-Viz_LIB_INCLUDE_DIRECTORIES} )

# Add Ot5 include dirs
foreach( I IN LISTS EPL_VIZ_QT_COMPONENTS )
  include_directories( ${Qt5${I}_INCLUDE_DIRS} )
endforeach( I IN LISTS EPL_VIZ_QT_COMPONENTS )


if( NOT EXISTS ${PROJECT_SOURCE_DIR}/include )
  file( MAKE_DIRECTORY ${PROJECT_SOURCE_DIR}/include )
endif( NOT EXISTS ${PROJECT_SOURCE_DIR}/include )

configure_file( ${EPL_VIZ_TEMPLATES_DIR}/defines.in.hpp ${PROJECT_SOURCE_DIR}/include/defines.hpp @ONLY )

generate_format_command( format 3.9.1 )

######################
# Add subdirectories #
######################

foreach( I IN LISTS EPL-Viz_SUBDIR_LIST )
  add_subdirectory( "${I}" )
endforeach( I IN LISTS EPL-Viz_SUBDIR_LIST )
