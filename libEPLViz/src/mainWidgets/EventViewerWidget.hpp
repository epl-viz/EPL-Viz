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
 * \file EventViewerWidget.hpp
 */

#pragma once

#include "CaptureInstance.hpp"
#include "EventLog.hpp"
#include <QTreeWidget>

namespace EPL_Viz {

class MainWindow;

class EventViewerWidget : public QTreeWidget {
  Q_OBJECT

 private:
  uint32_t                   appID = UINT32_MAX;
  EPL_DataCollect::EventLog *log   = nullptr;
  MainWindow *               mw    = nullptr;

 public:
  EventViewerWidget(QWidget *parent = nullptr);
  ~EventViewerWidget()              = default;

  void updateEvents();
  void setMainWindow(MainWindow *mainWin) { mw = mainWin; }

 private slots:
  void jumpToEvent(QTreeWidgetItem *item);

 public slots:
  void start(EPL_DataCollect::CaptureInstance *ci);
  void reset();

 signals:
  void eventSelected(uint32_t cycle);
};
}
