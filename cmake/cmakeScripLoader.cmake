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


# load the cmakeBuildTools
set( BUILD_TOOLS_SCRIPT_PATH "${CMAKE_CURRENT_LIST_DIR}/cmakeBuildTools/cmakeBuildTools.cmake" )

if( NOT EXISTS "${BUILD_TOOLS_SCRIPT_PATH}" )
  message( FATAL_ERROR "${BUILD_TOOLS_SCRIPT_PATH} does not exist. Try initializing the git submodules")
endif( NOT EXISTS "${BUILD_TOOLS_SCRIPT_PATH}" )

include( ${CMAKE_CURRENT_LIST_DIR}/cmakeBuildTools/cmakeBuildTools.cmake )

# include all list files

file( GLOB MODULE_LIST ${CMAKE_CURRENT_LIST_DIR}/*.cmake )

foreach( I IN LISTS MODULE_LIST )
  if( CMAKE_CURRENT_LIST_FILE STREQUAL I )
    continue()
  endif( CMAKE_CURRENT_LIST_FILE STREQUAL I )
  include( ${I} )
endforeach( I IN LISTS MODULE_LIST )
