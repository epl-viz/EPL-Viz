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

#pragma once

#include "PacketVizPacket.hpp"
#include <QWidget>
#include <qwt_scale_engine.h>
#include <qwt_scale_widget.h>
#include <vector>

namespace EPL_Viz {

class PacketVizModel;

class PacketVizWidget : public QWidget {
  Q_OBJECT
 private:
  QwtScaleEngine *scaleEngine = nullptr;
  QwtScaleWidget *scaleWidget = nullptr;

  QWidget *parentWidget = nullptr;

  PacketVizModel *                                           model = nullptr;
  std::vector<EPL_DataCollect::InputHandler::PacketMetadata> packetData;

  std::vector<PacketVizPacket *> packetWidgets;

  int maxTime = 0;

 protected:
  void resizeEvent(QResizeEvent *ev) override;

 public:
  explicit PacketVizWidget(QWidget *parent = 0);
  virtual ~PacketVizWidget();

  void setModel(PacketVizModel *m);
  void setMaxTime(int t);

  void redraw();
  void setPackets(std::vector<EPL_DataCollect::InputHandler::PacketMetadata> d);

 signals:

 public slots:
  void timeIndexChanged(int index);
};
}
