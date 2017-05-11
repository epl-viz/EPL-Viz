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

#include "SettingsWindow.hpp"
#include <InputHandler.hpp>
#include <QFrame>
#include <qwt/qwt_scale_draw.h>

namespace Ui {
class PacketVizPacket;
}

namespace EPL_Viz {

class PacketVizWidget;

class PacketVizPacket : public QFrame {
  Q_OBJECT
 public:
  enum HighlightMode { NONE, SELECTED };

 private:
  Ui::PacketVizPacket *ui;
  PacketVizWidget *    parentWidget = nullptr;
  uint64_t             pkgIndex;

  HighlightMode hMode = NONE;

  QColor calcBGColor(EPL_DataCollect::PacketType type, SettingsProfileItem::Config &cfg);

 protected:
  void mousePressEvent(QMouseEvent *event) override;

 public:
  explicit PacketVizPacket(QWidget *parent = 0);
  ~PacketVizPacket();

  void resizeAll(int x, int y);

  HighlightMode getHMode() { return hMode; }
  void setHMode(HighlightMode m);

  uint64_t getPkgIndex() const noexcept { return pkgIndex; }

  void setPacketData(EPL_DataCollect::InputHandler::PacketMetadata data,
                     int                                           relTime,
                     uint64_t                                      pIndex,
                     SettingsProfileItem::Config &                 cfg);
};
}
