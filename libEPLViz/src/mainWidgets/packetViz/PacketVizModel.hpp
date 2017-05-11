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

#include "BaseModel.hpp"
#include "PacketVizWidget.hpp"
#include "SettingsWindow.hpp"
#include <InputHandler.hpp>
#include <QComboBox>
#include <qwt_scale_engine.h>
#include <qwt_scale_widget.h>

namespace EPL_Viz {

class PacketVizModel : public BaseModel {
 public:
  enum CycleTimeing { CURRENT, MAX, AVERAGE };

 private:
  PacketVizWidget *                                          packetViz = nullptr;
  std::vector<EPL_DataCollect::InputHandler::PacketMetadata> dataToSet;

  CycleTimeing timeing          = CURRENT;
  int          averageCycleTime = 0;
  int          maxCycleTime     = 0;
  int          currentCycleTime = 0;

  QComboBox *timeSelector = nullptr;

 public:
  PacketVizModel(MainWindow *mw, PacketVizWidget *pvw, QComboBox *ts);
  virtual ~PacketVizModel();

  void update() override;
  void updateWidget() override;
  void init() override;

  QString getName() override { return "PacketViz"; }

  void timeIndexChanged(int index);
};
}
