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
 * \file qwtplotmodel.cpp
 */

#include "qwtplotmodel.hpp"
#include "QPointF"
#include "mainwindow.hpp"
using namespace EPL_Viz;
using namespace EPL_DataCollect;

QWTPlotModel::QWTPlotModel(MainWindow *win) : BaseModel() {
  window = win;
  curve  = new QwtPlotCurve();
}

QWTPlotModel::~QWTPlotModel() { delete curve; }

void QWTPlotModel::init() {
  plot = window->findChild<QwtPlot *>("qwtPlot");
  connect(this, SIGNAL(requestRedraw()), plot, SLOT(repaint()));

  CaptureInstance *ci     = window->getCaptureInstance();
  Cycle *          startC = ci->getStartCycle();
  auto *           cs     = startC->getCycleStorage(EPL_DataCollect::constants::EPL_DC_PLUGIN_TIME_SERIES_CSID);
  tsp                     = dynamic_cast<plugins::CSTimeSeriesPtr *>(cs);
}

// TODO: Connect this slot to the signal of the OD Widget
void QWTPlotModel::createPlot(uint8_t nodeID, uint16_t index, uint8_t subIndex) {
  timeSeries = std::make_shared<plugins::TimeSeries>(nodeID, index, subIndex);
  tsp->addTS(timeSeries);

  curve->attach(plot);
}

void QWTPlotModel::update(Cycle *cycle) {
  (void)cycle;
  // We're using always the newest cycle
  Cycle curCycle = window->getCaptureInstance()->getCycleContainer()->pollCycle();

  std::vector<double> values       = timeSeries->tsData;
  size_t              oldDataCount = curve->data()->size();
  size_t              newDataCount = values.size();
  size_t              start        = 0;

  if (oldDataCount == newDataCount)
    return;
  if (oldDataCount < newDataCount)
    start = oldDataCount;

  for (size_t i = start; i < newDataCount; ++i) {
    double x = static_cast<double>(i);
    double y = static_cast<double>(values[i]);
    curve->setSamples(&x, &y, 1);
  }
  emit requestRedraw();
}
