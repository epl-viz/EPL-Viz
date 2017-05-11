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

#include "PacketVizModel.hpp"
#include "EPLVizEnum2Str.hpp"
#include "MainWindow.hpp"

using namespace EPL_Viz;
using namespace std;
using namespace chrono;

PacketVizModel::PacketVizModel(MainWindow *mw, PacketVizWidget *pvw, QComboBox *ts) : BaseModel(mw, pvw) {
  packetViz    = pvw;
  timeSelector = ts;
  timeSelector->addItem("Current Cycle", static_cast<int>(CURRENT));
  //  timeSelector->addItem("Max Cycle", static_cast<int>(MAX));
  //  timeSelector->addItem("Average Cycle", static_cast<int>(AVERAGE));
  packetViz->setModel(this);
  packetViz->setMaxTime(1);
}

PacketVizModel::~PacketVizModel() {}

void PacketVizModel::init() {
  packetViz->setMaxTime(1);
  packetViz->setPackets({}, 0);
}

void PacketVizModel::packetSelected(uint64_t pkg) { emit packetChanged(pkg); }

void PacketVizModel::update() {
  ProtectedCycle &cycle = BaseModel::getCurrentCycle();
  auto            lock  = cycle.getLock();

  auto packets = cycle->getPackets();

  if (packets.empty())
    return;

  dataToSet.clear();
  startPacketIndex = packets.front().getPacketIndex();
  stopPacketIndex  = packets.back().getPacketIndex();

  // Autoselect first packet on update
  auto maxPackets = getMainWindow()->getSettingsWin()->getConfig().packetVizMaxPackets;

  { // No scope so that metaData can be destroyed ASAP
    auto metaData = getMainWindow()->getCaptureInstance()->getInputHandler()->getPacketsMetadata();
    for (size_t i = startPacketIndex;
         i < metaData->size() && (i - startPacketIndex) <= packets.size() && (i - startPacketIndex) <= maxPackets;
         ++i) {
      auto tempData = metaData->at(i);
      dataToSet.emplace_back(tempData);
    }
  }

  if (dataToSet.empty())
    return;

  int64_t diff     = dataToSet.back().timeStamp - dataToSet.front().timeStamp;
  currentCycleTime = static_cast<int>(diff / 1000);
  maxCycleTime     = currentCycleTime;
  averageCycleTime = maxCycleTime;

  switch (timeing) {
    case CURRENT: packetViz->setMaxTime(currentCycleTime); break;
    case MAX: packetViz->setMaxTime(maxCycleTime); break;
    case AVERAGE: packetViz->setMaxTime(averageCycleTime); break;
  }
}

void PacketVizModel::packetHasChanged(uint64_t pkg) { packetViz->setSelectedPacket(pkg); }

void PacketVizModel::updateWidget() {
  packetViz->redraw();
  packetViz->setPackets(dataToSet, startPacketIndex);

  if (!(packetViz->getSelectedPacket() >= startPacketIndex && packetViz->getSelectedPacket() <= stopPacketIndex))
    packetViz->setSelectedPacket(startPacketIndex);
}

void PacketVizModel::timeIndexChanged(int index) {
  qDebug() << EPLVizEnum2Str::toStr(static_cast<CycleTimeing>(timeSelector->itemData(index).toInt())).c_str();
}
