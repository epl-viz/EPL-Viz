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
 * \file TimeLineModel.hpp
 */

#pragma once

#include "BaseModel.hpp"
#include "Cycle.hpp"
#include "QwtBaseModel.hpp"
#include "TimeLineMagnifier.hpp"

#include <qwt_picker_machine.h>
#include <qwt_plot_marker.h>
#include <qwt_plot_picker.h>

#include <QScrollBar>

namespace EPL_Viz {
class TimeLineModel : public QwtBaseModel {
  Q_OBJECT

 private:
  uint32_t                   appid;
  double                     defaultWheelFactor;
  EPL_DataCollect::EventLog *log;
  QScrollBar *               scrollbar;
  QwtPlotPicker *            point;
  QwtPlotPicker *            area;


  QList<std::shared_ptr<QwtPlotMarker>> markers;
  QwtPlotMarker                         curCycleMarker;
  QwtPlotMarker                         newestCycleMarker;

  std::unique_ptr<TimeLineMagnifier> magnifier;

 public:
  const static uint32_t DEF_VIEWPORT_SIZE = 10;

  TimeLineModel(MainWindow *mw, QwtPlot *widget);
  TimeLineModel() = delete;
  virtual ~TimeLineModel();

  void init() override;

 protected:
  void update() override;
  void updateWidget() override;

 public slots:
  void updateViewport(int value);
  void reset();
  void pointSelected(const QPointF &pa);
  void createPlot(uint8_t nodeID, uint16_t index, uint16_t subIndex, std::string cs, QColor color);
};
}
