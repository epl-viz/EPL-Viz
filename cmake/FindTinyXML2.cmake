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

# Set TinyXML2_ROOT if not found

include(FindPackageHandleStandardArgs)

find_path(
  TinyXML2_INC
    NAMES tinyxml2.h
    HINTS
      ${TinyXML2_ROOT}/include
      ENV TinyXML2_ROOT
)

find_library(
  TinyXML2_LIB
    NAMES tinyxml2
    HINTS
      ${TinyXML2_ROOT}/lib
      ENV TinyXML2_ROOT
)

set( REPLACE_PREFIX ".*static[ \t\n]+const[ \t\n]+int[ \t\n]+TIXML2_" )
set( REPLACE_SUFFIX "_VERSION[ \t\n]*=[ \t\n]*([0-9]+)[ \t\n]*;.*" )

if( TinyXML2_INC AND EXISTS "${TinyXML2_INC}/tinyxml2.h" )
  file( READ "${TinyXML2_INC}/tinyxml2.h" TinyXML2_INC_FILE )
  string( REGEX REPLACE "${REPLACE_PREFIX}MAJOR${REPLACE_SUFFIX}" "\\1" TinyXML2_VERSION_MAJOR "${TinyXML2_INC_FILE}" )
  string( REGEX REPLACE "${REPLACE_PREFIX}MINOR${REPLACE_SUFFIX}" "\\1" TinyXML2_VERSION_MINOR "${TinyXML2_INC_FILE}" )
  string( REGEX REPLACE "${REPLACE_PREFIX}PATCH${REPLACE_SUFFIX}" "\\1" TinyXML2_VERSION_PATCH "${TinyXML2_INC_FILE}" )

  set( TinyXML2_VERSION "${TinyXML2_VERSION_MAJOR}.${TinyXML2_VERSION_MINOR}.${TinyXML2_VERSION_PATCH}" )
endif( TinyXML2_INC AND EXISTS "${TinyXML2_INC}/tinyxml2.h" )

find_package_handle_standard_args(
  TinyXML2
  VERSION_VAR TinyXML2_VERSION
  REQUIRED_VARS TinyXML2_INC TinyXML2_LIB
)

if( TinyXML2_FOUND )
  set( TinyXML2_INCLUDE_DIRS ${TinyXML2_INC} )
  set( TinyXML2_LIBRARIES    ${TinyXML2_LIB} )

  if( NOT TARGET TinyXML2::TinyXML2 )
    add_library(TinyXML2::TinyXML2 UNKNOWN IMPORTED)
    set_target_properties( TinyXML2::TinyXML2
      PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${TinyXML2_INCLUDE_DIRS}"
        IMPORTED_LOCATION             "${TinyXML2_LIBRARIES}"
    )
  endif( NOT TARGET TinyXML2::TinyXML2)
endif()
