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

using namespace EPL_Viz;
using namespace std;
using namespace chrono;

PacketVizModel::PacketVizModel(MainWindow *mw, PacketVizWidget *pvw, QComboBox *ts) : BaseModel(mw, pvw) {
  packetViz    = pvw;
  timeSelector = ts;
  timeSelector->addItem("Current Cycle", static_cast<int>(CURRENT));
  timeSelector->addItem("Max Cycle", static_cast<int>(MAX));
  timeSelector->addItem("Average Cycle", static_cast<int>(AVERAGE));
  packetViz->setModel(this);
}

PacketVizModel::~PacketVizModel() {}

void PacketVizModel::update() {
  ProtectedCycle &cycle = BaseModel::getCurrentCycle();
  auto            lock  = cycle.getLock();

  auto packets = cycle->getPackets();

  if (packets.empty())
    return;

  microseconds diff = duration_cast<microseconds>(packets.back().getTimeStamp() - packets.front().getTimeStamp());
  currentCycleTime  = static_cast<int>(diff.count());
  maxCycleTime      = currentCycleTime;
  averageCycleTime  = maxCycleTime;

  switch (timeing) {
    case CURRENT: packetViz->setMaxTime(currentCycleTime); break;
    case MAX:
    case AVERAGE: break;
  }
}

void PacketVizModel::updateWidget() { packetViz->redraw(); }

void PacketVizModel::timeIndexChanged(int index) {
  qDebug() << EPLVizEnum2Str::toStr(static_cast<CycleTimeing>(timeSelector->itemData(index).toInt())).c_str();
}
