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
#include <QDebug>
#include <string>
#include <vector>
using namespace EPL_Viz;
using namespace EPL_DataCollect;
using namespace std;

PacketHistoryModel::PacketHistoryModel(MainWindow *window, QPlainTextEdit *widget) : BaseModel(window, widget) {
  (void)window;

  textBox = widget;
}

void PacketHistoryModel::init() {
  selectedPacket = UINT64_MAX;
  currentCycle   = nullptr;
  textBox->setPlainText("No Packets available yet.");
}

void PacketHistoryModel::update(ProtectedCycle &cycle) {
  currentCycle = &cycle;
  changePacket(UINT64_MAX);
}

void PacketHistoryModel::changePacket(uint64_t packet) {
  // Prevent wrong updates
  if (selectedPacket == packet || !currentCycle) {
    return;
  }

  selectedPacket = packet;

  auto lock = currentCycle->getLock();

  Cycle *cycle = currentCycle->getCycle();

  if (!cycle)
    return;

  vector<Packet> packets = cycle->getPackets();
  string         text;

  if (selectedPacket == UINT64_MAX)
    text = "Please select a packet in the Cycle Commands widget.";
  else if (selectedPacket < packets.size())
    text = packets[selectedPacket].getWiresharkString();
  else
    text = "Packet out of bounds";

  textBox->setPlainText(QString::fromStdString(text));
}
