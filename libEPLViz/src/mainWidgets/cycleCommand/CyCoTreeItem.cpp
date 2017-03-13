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
/*!
 * \file CyCoTreeItem.cpp
 */
#include "CyCoTreeItem.hpp"
#include "EPLEnum2Str.hpp"

using namespace EPL_DataCollect;
using namespace EPL_Viz;
using namespace std::chrono;

CyCoTreeItem::CyCoTreeItem(TreeModelItemBase *parent, ProtectedCycle &cycle, size_t packetIndexs)
    : TreeModelItemBase(parent), c(cycle), pIndex(packetIndexs) {}

CyCoTreeItem::~CyCoTreeItem() {}

Qt::ItemFlags CyCoTreeItem::flags() { return Qt::ItemIsEnabled; }
bool          CyCoTreeItem::hasChanged() { return false; }

QVariant CyCoTreeItem::dataDisplay(int column) {
  auto lock = c.getLock();

  if (pIndex >= c->getPackets().size())
    return QVariant();

  Packet                   packet = c->getPackets().at(pIndex);
  system_clock::time_point SoC    = c->getPackets().at(0).getTimeStamp();
  system_clock::time_point last;
  if (pIndex == 0) {
    last = SoC;
  } else {
    last = c->getPackets().at(pIndex - 1).getTimeStamp();
  }

  switch (column) {
    case 0: // TYPE
      return QVariant(EPLEnum2Str::toStr(packet.getType()).c_str());
    case 1: // SOURCE
      return QVariant(static_cast<int>(packet.getSrcNode()));
    case 2: // DEST
      return QVariant(static_cast<int>(packet.getDestNode()));
    case 3: // Relative SoC
      return QVariant(std::to_string((packet.getTimeStamp() - SoC).count()).c_str());
    case 4: // Relative lasts
      return QVariant(std::to_string((packet.getTimeStamp() - last).count()).c_str());
    default: return QVariant();
  }
}

QVariant CyCoTreeItem::dataTooltip(int column) {
  switch (column) {
    case 0: // TYPE
      return QVariant("The packet type of the current entry");
    case 1: // SOURCE
      return QVariant("The source of the current packet");
    case 2: // DEST
      return QVariant("The destination of the current packet");
    case 3: // Relative SoC
      return QVariant("The time passed since the last SoC");
    case 4: // Relative lasts
      return QVariant("The time passed since the last packet");
    default: return QVariant();
  }
}

QVariant CyCoTreeItem::data(int column, Qt::ItemDataRole role) {
  if (role != Qt::DisplayRole)
    return QVariant();

  switch (role) {
    case Qt::DisplayRole: return dataDisplay(column);
    case Qt::ToolTipRole: return dataTooltip(column);
    default: return QVariant();
  }
}
