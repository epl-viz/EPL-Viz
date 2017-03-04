/*!
  * \file EPLVizEnum2Str.cpp
  * \warning This is an automatically generated file!
  */

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcovered-switch-default"

#include "EPLVizEnum2Str.hpp"

// clang-format off

namespace EPL_Viz {

/*!
 * \brief Converts the enum GUIState to a std::string 
 * \param _var The enum value to convert
 * \returns _var converted to a std::string 
 */
std::string EPLVizEnum2Str::toStr( GUIState _var ) noexcept {
  switch ( _var ) {
    case GUIState::UNINIT:    return "UNINIT";
    case GUIState::RECORDING: return "RECORDING";
    case GUIState::PAUSED:    return "PAUSED";
    case GUIState::STOPPED:   return "STOPPED";
    case GUIState::PLAYING:   return "PLAYING";
    default:                  return "<UNKNOWN>";
  }
}


}
// clang-format on

#pragma clang diagnostic pop
