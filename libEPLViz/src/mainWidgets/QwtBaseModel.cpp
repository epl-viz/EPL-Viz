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
 * \file QwtBaseModel.cpp
 */

#include "QwtBaseModel.hpp"
#include "MainWindow.hpp"
#include "QPointF"

using namespace EPL_Viz;
using namespace EPL_DataCollect;

QwtBaseModel::QwtBaseModel(MainWindow *win, QwtPlot *widget) : BaseModel(win, widget) {
  window = win;
  plot   = widget;
}

void QwtBaseModel::init() {
  connect(this, SIGNAL(requestRedraw()), plot, SLOT(repaint()));

  CaptureInstance *ci     = window->getCaptureInstance();
  Cycle *          startC = ci->getStartCycle();
  auto *           cs     = startC->getCycleStorage(EPL_DataCollect::constants::EPL_DC_PLUGIN_TIME_SERIES_CSID);
  tsp                     = dynamic_cast<plugins::CSTimeSeriesPtr *>(cs);
}

// TODO: Connect this slot to the signal of the OD Widget
void QwtBaseModel::createPlot(uint8_t nodeID, uint16_t index, uint8_t subIndex) {
  curve      = std::make_shared<QwtPlotCurve>();
  timeSeries = std::make_shared<plugins::TimeSeries>(nodeID, index, subIndex);
  tsp->addTS(timeSeries);

  curve->attach(plot);
  created = true;
}

void QwtBaseModel::update(ProtectedCycle &cycle) {
  // Abort when the QWTPlot has not been created
  if (!created)
    return;

  (void)cycle;

  // We're only using the newest cycle
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

void QwtBaseModel::setXMin(uint32_t min) {
  plot->setAxisScale(QwtPlot::xBottom, static_cast<double>(min), plot->axisScaleDiv(QwtPlot::xBottom).upperBound());
}

void QwtBaseModel::setXMax(uint32_t max) {
  plot->setAxisScale(QwtPlot::xBottom, plot->axisScaleDiv(QwtPlot::xBottom).lowerBound(), static_cast<double>(max));
}
