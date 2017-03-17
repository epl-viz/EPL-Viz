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
#include "PlotCreator.hpp"
#include "QPointF"
#include "QtDebug"

using namespace EPL_Viz;
using namespace EPL_DataCollect;
using namespace EPL_DataCollect::constants;

QwtBaseModel::QwtBaseModel(MainWindow *win, QwtPlot *widget) : BaseModel(win, widget) {
  window    = win;
  plot      = widget;
  maxXValue = 50;
  setup     = false;
  odTS      = true;
}

void QwtBaseModel::init() { connect(this, SIGNAL(requestRedraw()), plot, SLOT(repaint())); }

void QwtBaseModel::createPlot(uint8_t nodeID, uint16_t mainIndex, uint16_t subIndex) {
  if (setup) {
    qDebug() << "Already created";
    return;
  }
  curve = std::make_shared<QwtPlotCurve>();
  if (subIndex > UINT8_MAX)
    subIndex = 0;

  node     = nodeID;
  index    = mainIndex;
  subindex = subIndex;

  initTS();

  curve->attach(plot);
}

void QwtBaseModel::initTS() {
  qDebug() << "Initializing Timeseries with " + QString::number(node) + "/" + QString::number(index) + "/" +
                    QString::number(subindex);
  auto              ptr = window->getCaptureInstance()->getCycleContainer()->pollCyclePTR();
  CycleStorageBase *cs;

  if (window->getCaptureInstance()->getState() != CaptureInstance::CIstate::RUNNING) {
    cs = window->getCaptureInstance()->getStartCycle()->getCycleStorage(EPL_DC_PLUGIN_TIME_SERIES_CSID);
  } else {
    cs = ptr->getCycleStorage(EPL_DC_PLUGIN_TIME_SERIES_CSID);
  }

  auto *tsp = dynamic_cast<plugins::CSTimeSeriesPtr *>(cs);
  if (odTS) {
    qDebug() << "Setting timeseries hardcoded";
    timeSeries = std::make_shared<plugins::TimeSeries>(node, index, subindex);
  } else
    timeSeries = std::make_shared<plugins::TimeSeries>(node, csName);
  tsp->addTS(timeSeries);

  created = true;
}


void QwtBaseModel::replot() {
  postToThread([&] { plot->replot(); }, plot);
  emit requestRedraw();
}

void QwtBaseModel::update(ProtectedCycle &cycle) {
  (void)cycle;
  // If setup widget has been used used, initialize timeseries now
  if (setup) {
    initTS();

    curve = std::make_shared<QwtPlotCurve>();
    curve->attach(plot);

    setup = false;
  }
  // Abort when the QWTPlot has not been created
  if (!created) {
    replot();
    return;
  }

  size_t oldDataCount = curve->dataSize();
  size_t newDataCount = timeSeries->tsData.size();
  qDebug() << "Updating BaseModel with timeseries data of the size " + QString::number(newDataCount) + "and " +
                    QString::number(oldDataCount) + " old values";
  if (oldDataCount == newDataCount)
    return;

  std::vector<double> xValues(newDataCount);
  for (size_t i = 0; i < newDataCount; ++i) {
    xValues[i] = static_cast<double>(i);
  }

  curve->setSamples(xValues.data(), timeSeries->tsData.data(), static_cast<int>(newDataCount));
  curve->attach(plot);

  replot();
}

void QwtBaseModel::setXMin(uint32_t min) {
  postToThread(
        [&] {
          plot->setAxisScale(
                QwtPlot::xBottom, static_cast<double>(min), plot->axisScaleDiv(QwtPlot::xBottom).upperBound());
        },
        plot);
}

void QwtBaseModel::setXMax(uint32_t max) {
  postToThread(
        [&] {
          plot->setAxisScale(
                QwtPlot::xBottom, plot->axisScaleDiv(QwtPlot::xBottom).lowerBound(), static_cast<double>(max));
        },
        plot);
}

void QwtBaseModel::setupPlotting() {
  PlotCreator::PlotCreatorData data = PlotCreator::getNewPlot();
  if (data.isOK) {
    node     = data.node;
    index    = data.index;
    subindex = data.subIndex;
    odTS     = data.defaultODPlot;
    csName   = data.csName;

    setup = true;
  }
}
