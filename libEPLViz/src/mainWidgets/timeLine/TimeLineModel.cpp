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
 * \file TimeLineModel.cpp
 */

#include "TimeLineModel.hpp"
#include "MainWindow.hpp"
using namespace EPL_Viz;
using namespace EPL_DataCollect;

TimeLineModel::TimeLineModel(MainWindow *mw, QwtPlot *widget) : QwtBaseModel(mw, widget) {
  (void)mw;
  (void)widget;
}

TimeLineModel::~TimeLineModel() {}

void TimeLineModel::init() {
  markers.clear();
  viewportSize = 1000;
  curCycleMarker.setLineStyle(QwtPlotMarker::VLine);
  curCycleMarker.setLinePen(QColor(0,0,0), 2, Qt::PenStyle::DotLine);
  curCycleMarker.setXValue(static_cast<double>(0));
  curCycleMarker.attach(plot);
  log   = getMainWindow()->getCaptureInstance()->getEventLog();
  appid = log->getAppID();

  QwtBaseModel::init();
}

void TimeLineModel::update(ProtectedCycle &cycle) {
  if (!created)
    return;

  // Change cyclemarker position
  curCycleMarker.setXValue(static_cast<double>(window->getCycleNum()));

  // Add new markers
  std::vector<EventBase *> nEvents = log->pollEvents(appid);
  for (EventBase *ev : nEvents) {
    std::shared_ptr<QwtPlotMarker> marker =
          std::make_shared<QwtPlotMarker>(QString::fromStdString(ev->getDescription()));

    marker->setLineStyle(QwtPlotMarker::VLine);

    uint32_t x;
    ev->getCycleRange(&x);
    marker->setXValue(static_cast<double>(x));

    marker->attach(plot);
    markers.append(marker);
  }

  QwtBaseModel::update(cycle);
}

void TimeLineModel::updateViewport(int value) {
  int64_t nmin = (static_cast<int64_t>(value) - viewportSize/2);
  int64_t nmax = (static_cast<int64_t>(value) + viewportSize/2);

  qDebug() << "Changing viewport to [" + QString::number(nmin) + "-" + QString::number(nmax) + "]";

  if (nmin < 0) {
    nmax += std::abs(nmin);
    nmin  = 0;
  }

  uint32_t max =  static_cast<uint32_t>(plot->axisScaleDiv(QwtPlot::xBottom).upperBound());
  if (nmax > max) {
    nmin -= (nmax - max);
    nmax  = max;
  }
  plot->setAxisScale(QwtPlot::xBottom, static_cast<double>(nmin), static_cast<double>(nmax));
  plot->replot();
}
