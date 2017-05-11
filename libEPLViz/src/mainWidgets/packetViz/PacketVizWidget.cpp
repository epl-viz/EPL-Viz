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

#include "PacketVizWidget.hpp"
#include "MainWindow.hpp"
#include "PacketVizModel.hpp"

using namespace EPL_Viz;
using namespace EPL_DataCollect;

PacketVizWidget::PacketVizWidget(QWidget *parent) : QWidget(parent) {
  parentWidget = parent;

  scaleEngine = new QwtLinearScaleEngine();
  scaleWidget = new QwtScaleWidget(QwtScaleDraw::TopScale, parent);

  scaleWidget->setScaleDiv(scaleEngine->divideScale(0, 1000, 10, 10));
  scaleWidget->setBorderDist(3, 3);
  scaleWidget->setTitle("Time passed in μs");
  scaleWidget->show();
}

PacketVizWidget::~PacketVizWidget() {
  for (auto *i : packetWidgets)
    delete i;

  packetWidgets.clear();
}

void PacketVizWidget::redraw() {
  scaleWidget->setGeometry(0, 0, parentWidget->size().width(), 30);
  scaleWidget->setScaleDiv(scaleEngine->divideScale(0, maxTime, 10, 10));

  if (packetData.empty())
    return;

  auto  cfg     = model->getMainWindow()->getSettingsWin()->getConfig();
  QSize winSize = size();

  int startY = static_cast<int>(cfg.packetVizScaleHeight);
  int endY   = winSize.height();

  int64_t startP = packetData[0].timeStamp;
  for (size_t i = 0; i < packetData.size() - 1; ++i) {
    int64_t startRel = packetData[i].timeStamp - startP;
    int64_t endRel   = packetData[i + 1].timeStamp - startP;
    bool    isMN     = packetData[i].getFiled(InputHandler::PacketMetadata::SOURCE) == 240;

    startRel /= 1000; // Convert to μs
    endRel /= 1000;   // Convert to μs

    int startX = static_cast<int>(scaleWidget->scaleDraw()->labelPosition(static_cast<double>(startRel)).rx());
    int endX   = static_cast<int>(scaleWidget->scaleDraw()->labelPosition(static_cast<double>(endRel)).rx());

    startX = startX < winSize.width() ? startX : winSize.width();
    endX   = endX < winSize.width() ? endX : winSize.width();

    packetWidgets[i]->move(startX, isMN ? startY : startY + (endY - startY) / 2);
    packetWidgets[i]->resize(endX - startX, (endY - startY) / 2);
  }
}


void PacketVizWidget::setPackets(std::vector<EPL_DataCollect::InputHandler::PacketMetadata> d) {
  packetData = d;

  if (packetData.empty()) {
    for (auto *i : packetWidgets)
      i->hide();

    redraw();
    return;
  }

  auto cfg = model->getMainWindow()->getSettingsWin()->getConfig();

  size_t i;
  for (i = 0; i < packetData.size() - 1; ++i) {
    if (i >= packetWidgets.size()) {
      packetWidgets.emplace_back(new PacketVizPacket(this));
    }

    packetWidgets[i]->setPacketData(
          packetData[i], static_cast<int>(packetData[i + 1].timeStamp - packetData[i].timeStamp) / 1000, cfg);
    packetWidgets[i]->show();
  }

  for (size_t j = i; j < packetWidgets.size(); ++j) {
    packetWidgets[j]->hide();
  }

  redraw();
}

void PacketVizWidget::resizeEvent(QResizeEvent *ev) {
  QWidget::resizeEvent(ev);
  redraw();
}

void PacketVizWidget::timeIndexChanged(int index) {
  if (model)
    model->timeIndexChanged(index);
}

void PacketVizWidget::setModel(PacketVizModel *m) { model = m; }
void PacketVizWidget::setMaxTime(int t) { maxTime = t; }
