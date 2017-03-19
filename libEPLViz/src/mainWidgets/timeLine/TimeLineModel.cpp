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
#include "PlotCreator.hpp"
#include "TimeLineWidget.hpp"
using namespace EPL_Viz;
using namespace EPL_DataCollect;

TimeLineModel::TimeLineModel(MainWindow *mw, QwtPlot *widget) : QwtBaseModel(mw, widget) {
  (void)mw;
  (void)widget;
  scrollbar = mw->findChild<QScrollBar *>("scrBarTimeline");

  widget->enableAxis(QwtPlot::xBottom, false);

  widget->setAxisScale(QwtPlot::yLeft, 0, 10);
  widget->setAxisAutoScale(QwtPlot::yLeft, true);

  widget->setAxisScale(QwtPlot::xTop, 0, DEF_VIEWPORT_SIZE);
  widget->setAxisAutoScale(QwtPlot::xTop, true);

  magnifier = std::make_unique<TimeLineMagnifier>(scrollbar, this, &maxXValue, widget->canvas());
  magnifier->setAxisEnabled(QwtPlot::xTop, true);
  magnifier->setAxisEnabled(QwtPlot::yLeft, false);
}

TimeLineModel::~TimeLineModel() {}

void TimeLineModel::init() {
  markers.clear();

  curCycleMarker.setLineStyle(QwtPlotMarker::VLine);
  curCycleMarker.setLinePen(QColor(0, 0, 0), 2, Qt::PenStyle::DotLine);
  curCycleMarker.setXAxis(QwtPlot::xTop);
  curCycleMarker.setXValue(static_cast<double>(0));
  curCycleMarker.setLabel(QwtText("View"));
  curCycleMarker.setLabelAlignment(Qt::AlignTop | Qt::AlignLeft);
  curCycleMarker.attach(plot);

  newestCycleMarker.setLineStyle(QwtPlotMarker::VLine);
  newestCycleMarker.setLinePen(QColor(255, 0, 0), 2, Qt::PenStyle::DotLine);
  newestCycleMarker.setXAxis(QwtPlot::xTop);
  newestCycleMarker.setXValue(static_cast<double>(0));
  newestCycleMarker.setLabel(QwtText("Backend"));
  newestCycleMarker.setLabelAlignment(Qt::AlignLeft);
  newestCycleMarker.attach(plot);


  log   = getMainWindow()->getCaptureInstance()->getEventLog();
  appid = log->getAppID();

  QwtBaseModel::init();
}

void TimeLineModel::update(ProtectedCycle &cycle) {
  // Change cyclemarker position
  uint32_t cycleNum = window->getCycleNum();
  if (cycleNum == UINT32_MAX)
    cycleNum = cycle->getCycleNum();
  curCycleMarker.setXValue(static_cast<double>(cycleNum));

  // Set newest Cycle marker
  uint32_t newest = window->getCaptureInstance()->getCycleContainer()->pollCycle().getCycleNum();
  newestCycleMarker.setXValue(static_cast<double>(newest));
  maxXValue = newest;
  emit maxValueChanged(0, static_cast<int>(maxXValue - getViewportSize()));
  scrollbar->setMaximum(static_cast<int>(maxXValue));

  // Add new markers
  std::vector<EventBase *> nEvents = log->pollEvents(appid);
  if (nEvents.size() > 0)
    qDebug() << "Number of new Events adding to timeline: " + QString::number(nEvents.size());
  for (EventBase *ev : nEvents) {

    std::shared_ptr<QwtPlotMarker> marker =
          std::make_shared<QwtPlotMarker>(QString::fromStdString(ev->getDescription()));

    marker->setLineStyle(QwtPlotMarker::VLine);

    QColor col;
    switch (ev->getType()) {
      case EvType::ERROR: col   = QColor(200, 0, 0); break;
      case EvType::WARNING: col = QColor(250, 250, 0); break;
      case EvType::INFO: col    = QColor(0, 200, 0); break;
      default: col              = QColor(0, 0, 0);
    }
    marker->setLinePen(col);

    uint32_t x;
    ev->getCycleRange(&x);
    marker->setXValue(static_cast<double>(x));
    marker->setXAxis(QwtPlot::xTop);

    marker->attach(plot);
    markers.append(marker);
  }

  QwtBaseModel::update(cycle);
}

void TimeLineModel::updateViewport(int value) {
  QwtScaleDiv scale = plot->axisScaleDiv(QwtPlot::xTop);
  double      upper = scale.upperBound();
  double      lower = scale.lowerBound();

  double viewportSize = upper - lower;

  if (viewportSize < 1)
    return;

  double min = static_cast<double>(value);
  double max = min + viewportSize;


  qDebug() << "Changing viewport to [" + QString::number(min) + "-" + QString::number(max) + "]";

  plot->setAxisScale(QwtPlot::xTop, min, max);
  replot();
}

void TimeLineModel::reset() {
  for (std::shared_ptr<QwtPlotMarker> marker : markers) {
    marker->detach();
  }
  markers.clear();
  curCycleMarker.detach();
  newestCycleMarker.detach();
  magnifier = std::make_unique<TimeLineMagnifier>(scrollbar, this, &maxXValue, plot->canvas());
  QwtBaseModel::reset();
}
