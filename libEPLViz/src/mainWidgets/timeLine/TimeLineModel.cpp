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
#include "QWTPlotWidget.hpp"
#include "SettingsProfileItem.hpp"
#include "TimeLineWidget.hpp"
#include <QApplication>

using namespace EPL_Viz;
using namespace EPL_DataCollect;

TimeLineModel::TimeLineModel(MainWindow *mainWin, QwtPlot *widget, QWTPlotModel *plotModel)
    : QwtBaseModel(mainWin, widget), mw(mainWin) {
  scrollbar = mw->findChild<QScrollBar *>("scrBarTimeline");

  widget->enableAxis(QwtPlot::xBottom, false);

  widget->setAxisScale(QwtPlot::yLeft, 0, 10);
  widget->setAxisAutoScale(QwtPlot::yLeft, true);

  widget->setAxisScale(QwtPlot::xTop, 0, DEF_VIEWPORT_SIZE);
  // widget->setAxisAutoScale(QwtPlot::xTop, true);

  magnifier = std::make_unique<TimeLineMagnifier>(scrollbar, this, &maxXValue, widget->canvas());
  magnifier->setAxisEnabled(QwtPlot::xTop, true);
  magnifier->setAxisEnabled(QwtPlot::yLeft, false);
  magnifier->setMouseButton(Qt::MouseButton::NoButton, Qt::KeyboardModifier::NoModifier);
  defaultWheelFactor = magnifier->wheelFactor();
  if (mw->getSettingsWin()->getConfig().invertTimeLineZoom)
    magnifier->setWheelFactor(1 / defaultWheelFactor);
  else
    magnifier->setWheelFactor(defaultWheelFactor);

  // Configure PlotPickers
  point = new QwtPlotPicker(plot->canvas());
  point->setAxis(QwtPlot::xTop, QwtPlot::yLeft);
  point->setStateMachine(new QwtPickerClickPointMachine());
  point->setMousePattern(QwtEventPattern::MousePatternCode::MouseSelect1, Qt::MouseButton::LeftButton);
  area = new QwtPlotPicker(plot->canvas());
  area->setAxis(QwtPlot::xTop, QwtPlot::yLeft);
  area->setStateMachine(new QwtPickerDragRectMachine());
  area->setMousePattern(QwtEventPattern::MousePatternCode::MouseSelect1, Qt::MouseButton::LeftButton);

  connect(point, SIGNAL(selected(QPointF)), this, SLOT(pointSelected(QPointF)));
  connect(area, SIGNAL(selected(QRectF)), plotModel, SLOT(changeArea(QRectF)));
}

TimeLineModel::~TimeLineModel() {
  delete point;
  delete area;
}

void TimeLineModel::pointSelected(const QPointF &pa) {
  uint32_t x = static_cast<uint32_t>(std::round(pa.x()));
  window->changeCycle(x);
  curCycleMarker.setXValue(x);
  setFitToPlot(false);
  replotPostMain();
}

void TimeLineModel::init() {
  markers.clear();

  curCycleMarker.setLineStyle(QwtPlotMarker::VLine);
  QColor col;
  if (QApplication::palette().background().color().lightness() < 128)
    col = QColor(240, 240, 240);
  else
    col = QColor(10, 10, 10);
  curCycleMarker.setLinePen(col, 2, Qt::PenStyle::DotLine);
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

void TimeLineModel::update() {
  ProtectedCycle &cycle = BaseModel::getCurrentCycle();

  if (window->getSettingsWin()->getConfig().invertTimeLineZoom)
    magnifier->setWheelFactor(1 / defaultWheelFactor);
  else
    magnifier->setWheelFactor(defaultWheelFactor);

  // Change cyclemarker position
  uint32_t cycleNum = window->getCycleNum();
  if (cycleNum == UINT32_MAX) {
    // Failsafe for wrong number saved
    cycleNum = cycle->getCycleNum();
    if (cycleNum == UINT32_MAX)
      cycleNum = calcXMaximum();
  }
  curCycleMarker.setXValue(static_cast<double>(cycleNum));

  // Set newest Cycle marker
  uint32_t newest = calcXMaximum();
  newestCycleMarker.setXValue(static_cast<double>(newest));
  maxXValue = newest;

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
      case EvType::PROTO_ERROR: col    = mw->getSettingsWin()->getConfig().evProtoError; break;
      case EvType::ERROR: col          = mw->getSettingsWin()->getConfig().evError; break;
      case EvType::WARNING: col        = mw->getSettingsWin()->getConfig().evWarning; break;
      case EvType::INFO: col           = mw->getSettingsWin()->getConfig().evInfo; break;
      case EvType::DEBUG: col          = mw->getSettingsWin()->getConfig().evDebug; break;
      case EvType::PLUGIN_EV_TEXT: col = mw->getSettingsWin()->getConfig().evPText; break;
      default: col                     = QColor(0, 0, 0);
    }
    marker->setLinePen(col);

    uint32_t x;
    ev->getCycleRange(&x);
    marker->setXValue(static_cast<double>(x));
    marker->setXAxis(QwtPlot::xTop);

    marker->attach(plot);
    markers.append(marker);
  }

  QwtBaseModel::update();
}

void TimeLineModel::updateWidget() {
  double viewPort = getViewportSize();
  if (viewPort > 1) {
    int nMax = static_cast<int>(round(maxXValue - viewPort));
    if (nMax > 0) {
      scrollbar->setMaximum(nMax);
      scrollbar->setPageStep(static_cast<int>(viewPort));
    }
  }
  QwtBaseModel::updateWidget();
}

void TimeLineModel::updateViewport(int value) {
  double viewportSize = getViewportSize();

  if (viewportSize < 1) {
    qDebug() << "Viewport size is <1";
    return;
  }

  double min = static_cast<double>(value);
  double max = min + viewportSize;

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
  QwtBaseModel::reset();
  if (window->getSettingsWin()->getConfig().invertTimeLineZoom)
    magnifier->setWheelFactor(1 / defaultWheelFactor);
  else
    magnifier->setWheelFactor(defaultWheelFactor);
}

void TimeLineModel::createPlot(uint8_t     nodeID,
                               uint16_t    index,
                               uint16_t    subIndex,
                               std::string cs,
                               QColor      color,
                               bool        usedInTime,
                               bool        usedInPlot) {
  QwtBaseModel::createPlot(nodeID, index, subIndex, cs, QwtPlot::xTop, color, usedInTime, usedInPlot);
}

void TimeLineModel::fitToPlot() { setFitToPlot(true); }
