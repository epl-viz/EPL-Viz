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
 * \file QWTPlotModel.hpp
 */
#pragma once

#include "BaseModel.hpp"
#include "CSTimeSeriesPtr.hpp"
#include "EPLVizDefines.hpp"
#include "EventLog.hpp"
#include "QwtBaseModel.hpp"
#include "TimeSeries.hpp"
#include <qwt_plot.h>
#include <qwt_plot_curve.h>

namespace EPL_Viz {

class MainWindow;

class QWTPlotModel : public QwtBaseModel {
  Q_OBJECT

 public:
  QWTPlotModel(MainWindow *win, QwtPlot *widget);
  QWTPlotModel() = delete;
  virtual ~QWTPlotModel();

 public slots:
  void createPlot(uint8_t     nodeID,
                  uint16_t    index,
                  uint16_t    subIndex,
                  std::string cs,
                  QColor      color,
                  bool        usedInTime = true,
                  bool        usedInPlot = true) override;
  void changeArea(QRectF rect);
  void fitToPlot();
};
}
