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
 * \file PacketHistoryModel.cpp
 */

#include "PacketHistoryModel.hpp"
#include "Cycle.hpp"
#include "MainWindow.hpp"
#include "PacketHistoryWidget.hpp"
#include <QDebug>
#include <string>
#include <vector>
using namespace EPL_Viz;
using namespace EPL_DataCollect;
using namespace std;

PacketHistoryModel::PacketHistoryModel(MainWindow *window) : BaseModel() {
  selectedPacket = UINT64_MAX;
  textWindow     = window->findChild<QPlainTextEdit *>("packetHistoryTextEdit");
  // connect update Text
  connect(this,
          SIGNAL(textUpdated(QString, QPlainTextEdit *)),
          window->findChild<PacketHistoryWidget *>("dockPacketHistory"),
          SLOT(updatePacketHistoryLog(QString, QPlainTextEdit *)));
  needUpdate = true;
}

void PacketHistoryModel::update(ProtectedCycle &cycle) {
  if (!needUpdate)
    return;

  auto lock = cycle.getLock();

  vector<Packet> packets = cycle->getPackets();
  string         text;
  if (selectedPacket == UINT64_MAX)
    text = "Please select a packet in the Cycle Commands widget.";
  else if (selectedPacket < packets.size())
    text = packets[selectedPacket].getWiresharkString();
  else
    text = "Packet out of bounds";
  emit textUpdated(QString::fromStdString(text), textWindow);
  needUpdate = false;
  qDebug() << "updating PacketHistoryModel";
}

void PacketHistoryModel::changePacket(uint64_t packet) {
  needUpdate     = true;
  selectedPacket = packet;
}

void PacketHistoryModel::init() {}
