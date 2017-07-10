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

set( UI_DIR  "${CMAKE_CURRENT_LIST_DIR}/ui" )

if( NOT IS_DIRECTORY "${UI_DIR}" )
  message( FATAL_ERROR "UI dir '${UI_DIR}' not found!" )
endif( NOT IS_DIRECTORY "${UI_DIR}" )

file( GLOB UI_FILE_LIST "${UI_DIR}/*.ui" )

set( OUT_DIR "${PROJECT_BINARY_DIR}/${CM_CURRENT_EXE_LC}" )
message( STATUS "    - UI / RC include file output dir: ${OUT_DIR}" )

set( CM_GENERATED_UI_FILES "" )
set( CM_GENERATE_UI_CMD    "" )

foreach( I IN LISTS UI_FILE_LIST )
  file( RELATIVE_PATH UI_FILE_NAME "${UI_DIR}" "${I}" )
  string( REGEX REPLACE "^([^\.]+)\.ui" "ui_\\1.h" INC_NAME "${UI_FILE_NAME}" )

  set( OUTFILE "${OUT_DIR}/${INC_NAME}" )
  set( infile  "${I}" )

  string( APPEND CM_GENERATE_UI_CMD "add_custom_command(\n" )
  string( APPEND CM_GENERATE_UI_CMD "  OUTPUT ${OUTFILE}\n" )
  string( APPEND CM_GENERATE_UI_CMD "  COMMAND ${Qt5Widgets_UIC_EXECUTABLE}\n" )
  string( APPEND CM_GENERATE_UI_CMD "  ARGS -o ${OUTFILE} ${I}\n" )
  string( APPEND CM_GENERATE_UI_CMD "  MAIN_DEPENDENCY ${I} VERBATIM\n)\n\n" )

  string( APPEND CM_GENERATED_UI_FILES "\n   ${OUTFILE}" )
endforeach( I IN LISTS UI_FILE_LIST )

set( CM_GENERATED_UI_FILES "${CM_GENERATED_UI_FILES}" PARENT_SCOPE )
set( CM_GENERATE_UI_CMD    "${CM_GENERATE_UI_CMD}"    PARENT_SCOPE )
