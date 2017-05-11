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

void PacketHistoryModel::init() { textBox->clear(); }

void PacketHistoryModel::update() {}
void PacketHistoryModel::updateWidget() {
  ProtectedCycle &cycle     = BaseModel::getCurrentCycle();
  auto            lock      = cycle.getLock();
  uint32_t        currCycle = cycle->getCycleNum();

  // Don't reset if cycle didn't change
  if (lastCycle != currCycle) {
    auto packets = cycle->getPackets();
    if (!packets.empty())
      changePacket(packets.front().getPacketIndex());
  }
}

void PacketHistoryModel::changePacket(uint64_t pkg) {
  string text;

  {
    auto metaData = getMainWindow()->getCaptureInstance()->getInputHandler()->getPacketsMetadata();
    if (metaData->size() <= pkg) {
      qDebug() << "PKG " << pkg << " out of bounds";
      return;
    }
    lastCycle = metaData->at(pkg).cycleNum;
  }

  if (pkg == UINT64_MAX)
    text = "";
  else
    text = BaseModel::getMainWindow()->getCaptureInstance()->getInputHandler()->generateWiresharkString(pkg);

  textBox->setPlainText(QString::fromStdString(text));
}
