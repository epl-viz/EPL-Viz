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
  textBox = widget;
}

void PacketHistoryModel::init() {
  selectedPacket = UINT64_MAX;
  textBox->clear();
}

void PacketHistoryModel::update() {}
void PacketHistoryModel::updateWidget() {
  uint32_t currCycle = BaseModel::getCurrentCycle()->getCycleNum();

  // Don't reset if cycle didn't change
  if (lastCycle != currCycle && getMainWindow()->getCycleNum() != UINT32_MAX) {
    changePacket(UINT64_MAX);
    lastCycle = currCycle;
  }
}

void PacketHistoryModel::changePacket(uint64_t packet) {
  ProtectedCycle &cycle = BaseModel::getCurrentCycle();
  auto            lock  = cycle.getLock();

  // Prevent wrong updates
  if (selectedPacket == packet) {
    return;
  }

  selectedPacket = packet;

  vector<Packet> packets = cycle->getPackets();
  string         text;

  if (selectedPacket == UINT64_MAX)
    text = "";
  else if (selectedPacket < packets.size())
    text = packets[selectedPacket].getWiresharkString();
  else
    text = "Packet out of bounds";

  textBox->setPlainText(QString::fromStdString(text));
  lastCycle = cycle->getCycleNum();
}
