/* Copyright (c) 2017, EPL-Vizards
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the EPL-Vizards nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL EPL-Vizards BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "PacketListItem.hpp"
#include "EPLEnum2Str.hpp"
#include "MainWindow.hpp"

using namespace EPL_Viz;
using namespace EPL_DataCollect;

PacketListItem::PacketListItem(TreeModelItemBase *                           parent,
                               MainWindow *                                  mainWin,
                               EPL_DataCollect::InputHandler::PacketMetadata d,
                               size_t                                        ind)
    : TreeModelItemBase(parent), metaData(d), index(ind), mw(mainWin) {
  uint8_t tmp = metaData.getFiled(InputHandler::PacketMetadata::PACKET_TYPE);
  type        = static_cast<PacketType>(tmp);
  typeVar     = EPLEnum2Str::toStr(type).c_str();

  ASndServiceID asndID = static_cast<ASndServiceID>(metaData.getFiled(InputHandler::PacketMetadata::SERVICE_ID));

  source = static_cast<int>(metaData.getFiled(InputHandler::PacketMetadata::SOURCE));
  dest   = static_cast<int>(metaData.getFiled(InputHandler::PacketMetadata::DESTINATION));

  tmp = metaData.getFiled(InputHandler::PacketMetadata::NMT_STATE);
  switch (type) {
    case PacketType::START_OF_ASYNC:
    case PacketType::POLL_RESPONSE: state = EPLEnum2Str::toStr(static_cast<NMTState>(tmp)).c_str(); break;
    case PacketType::ASYNC_SEND:
      if (asndID != ASndServiceID::STATUS_RESPONSE && asndID != ASndServiceID::IDENT_RESPONSE) {
        break;
      }

      state = EPLEnum2Str::toStr(static_cast<NMTState>(tmp)).c_str();
      break;
    default: break;
  }

  tmp = metaData.getFiled(InputHandler::PacketMetadata::SERVICE_ID);
  switch (type) {
    case PacketType::START_OF_ASYNC: sID = EPLEnum2Str::toStr(static_cast<SoARequestServiceID>(tmp)).c_str(); break;
    case PacketType::ASYNC_SEND: sID     = EPLEnum2Str::toStr(static_cast<ASndServiceID>(tmp)).c_str(); break;
    default: break;
  }

  tmp = metaData.getFiled(InputHandler::PacketMetadata::COMMAND);
  if (type != PacketType::ASYNC_SEND)
    return;

  switch (asndID) {
    case ASndServiceID::SDO: cmdID         = EPLEnum2Str::toStr(static_cast<SDOCommandID>(tmp)).c_str(); break;
    case ASndServiceID::NMT_COMMAND: cmdID = EPLEnum2Str::toStr(static_cast<NMTCommand>(tmp)).c_str(); break;
    default: break;
  }
}

PacketListItem::~PacketListItem() {}

Qt::ItemFlags PacketListItem::flags() { return Qt::ItemIsEnabled | Qt::ItemNeverHasChildren; }
bool          PacketListItem::hasChanged() { return false; }

QVariant PacketListItem::dataDisplay(int column) {
  switch (column) {
    case 0: return QVariant(static_cast<uint32_t>(index + 1));
    case 1: return QVariant(metaData.cycleNum);
    case 2: return typeVar;
    case 3: return source;
    case 4: return dest;
    case 5: return state;
    case 6: return sID;
    case 7: return cmdID;
    default: return QVariant();
  }
}

QColor PacketListItem::dataBackground() {
  switch (type) {
    case PacketType::START_OF_CYCLE: return mw->getSettingsWin()->getConfig().pSoC;
    case PacketType::START_OF_ASYNC: return mw->getSettingsWin()->getConfig().pSoA;
    case PacketType::POLL_REQUEST: return mw->getSettingsWin()->getConfig().pPReq;
    case PacketType::POLL_RESPONSE: return mw->getSettingsWin()->getConfig().pPRes;
    case PacketType::ASYNC_SEND: return mw->getSettingsWin()->getConfig().pASnd;
    case PacketType::AINV: return mw->getSettingsWin()->getConfig().pAINV;
    case PacketType::AMNI: return mw->getSettingsWin()->getConfig().pANMI;
    case PacketType::UNDEF: return mw->getSettingsWin()->getConfig().pInvalid;
  }

  return QColor();
}

QColor PacketListItem::dataForground() {
  QColor c = dataBackground();
  if (!c.isValid())
    return QColor();

  if (c.lightness() >= 125) {
    return QColor("#000000");
  }

  return QColor("#ffffff");
}


QVariant PacketListItem::data(int column, Qt::ItemDataRole role) {
  QColor col;
  switch (role) {
    case Qt::DisplayRole: return dataDisplay(column);
    case Qt::BackgroundRole: col = dataBackground(); break;
    case Qt::ForegroundRole: col = dataForground(); break;
    default: return QVariant();
  }

  if (!col.isValid())
    return QVariant();

  return QBrush(col);
}
