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

#include <QAction>
#include <QMenu>
#include <QPointF>
#include <QtDebug>

using namespace EPL_Viz;
using namespace EPL_DataCollect;
using namespace EPL_DataCollect::constants;
using namespace std;

QwtBaseModel::QwtBaseModel(MainWindow *win, QwtPlot *widget) : BaseModel(win, widget) {
  window = win;
  plot   = widget;

  widget->setContextMenuPolicy(Qt::CustomContextMenu);
  widget->setAxisAutoScale(QwtPlot::yLeft, true);
  connect(widget, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));

  reset();
}

void QwtBaseModel::init() {
  updatePlotList();
  for (PlotCreator::PlotCreatorData data : registeredCurves) {
    createPlot(data.node, data.index, data.subIndex, data.csName, data.color);
  }
  registeredCurves.clear();
}

double QwtBaseModel::getViewportSize() {
  QwtScaleDiv div = plot->axisScaleDiv(QwtPlot::xTop);
  return div.upperBound() - div.lowerBound();
}

/**
 * @brief QwtBaseModel::replot Replots in the main Thread
 */
void QwtBaseModel::replot() {
  postToThread([&] { plot->replot(); }, plot);
}

void QwtBaseModel::update() {
  // Setting values in curves
  for (QPair<shared_ptr<QwtPlotCurve>, shared_ptr<plugins::TimeSeries>> pair : curves.values()) {
    shared_ptr<QwtPlotCurve>        curve      = pair.first;
    shared_ptr<plugins::TimeSeries> timeSeries = pair.second;

    size_t oldDataCount = curve->dataSize();
    size_t newDataCount = timeSeries->tsData.size();
    if (oldDataCount == newDataCount)
      return;

    xValues.reserve(newDataCount);
    for (size_t i = xValues.size(); i < newDataCount; ++i) {
      xValues[i] = static_cast<double>(i);
    }

    curve->setSamples(xValues.data(), timeSeries->tsData.data(), static_cast<int>(newDataCount));
  }

  replot();
}

void QwtBaseModel::updateWidget() {}

/**
 * @brief createStringIdentifier Returns an identifier for the index;
 * @param node node
 * @param index index
 * @param subIndex subindex
 * @param cs cycle storage name if needed.
 * @return Identifier
 */
QString QwtBaseModel::createStringIdentifier(uint8_t node, uint16_t index, uint16_t subIndex, std::string cs) {
  QString title;
  if (cs.empty())
    title = "Node: " + QString::number(node) + " Index: 0x" + QString::number(index, 16) + " SubIndex: 0x" +
            QString::number(subIndex, 16);
  else
    title = QString::fromStdString(cs);
  return title;
}

QString QwtBaseModel::createStringIdentifier(const PlotCreator::PlotCreatorData &data) {
  return createStringIdentifier(data.node, data.index, data.subIndex, data.csName);
}

/**
 * @brief QwtBaseModel::createPlot Method for creating a plot with the given values
 * Wether the CycleStorage name is empty or not, decides if the indices or the string should be used.
 * Calls initTTS()
 * @param nodeID ID of the node
 * @param mainIndex Index of the OD Entry
 * @param subIndex subindex to be plotted
 * @param csName CycleStorage name, ignored if empty
 */
void QwtBaseModel::createPlot(
      uint8_t nodeID, uint16_t mainIndex, uint16_t subIndex, std::string csName, QwtPlot::Axis axis, QColor color) {
  if (subIndex > UINT8_MAX)
    subIndex = 0;

  auto              ptr = window->getCaptureInstance()->getCycleContainer()->pollCyclePTR();
  CycleStorageBase *cs;

  if (window->getCaptureInstance()->getState() != CaptureInstance::CIstate::RUNNING) {
    cs = window->getCaptureInstance()->getStartCycle()->getCycleStorage(EPL_DC_PLUGIN_TIME_SERIES_CSID);
  } else {
    cs = ptr->getCycleStorage(EPL_DC_PLUGIN_TIME_SERIES_CSID);
  }

  auto *tsp = dynamic_cast<plugins::CSTimeSeriesPtr *>(cs);

  shared_ptr<plugins::TimeSeries> timeSeries;
  if (csName.empty())
    timeSeries = make_shared<plugins::TimeSeries>(nodeID, mainIndex, subIndex);
  else
    timeSeries = make_shared<plugins::TimeSeries>(nodeID, csName);
  tsp->addTS(timeSeries);

  shared_ptr<QwtPlotCurve> curve = make_shared<QwtPlotCurve>();
  curve->setXAxis(axis);
  curve->setYAxis(QwtPlot::yLeft);
  curve->setPen(color, curve->pen().width(), curve->pen().style());

  QString title = createStringIdentifier(nodeID, mainIndex, subIndex, csName);

  curve->setTitle(title);
  curve->attach(plot);

  curves.insert(title, QPair<shared_ptr<QwtPlotCurve>, shared_ptr<plugins::TimeSeries>>(curve, timeSeries));
}


void QwtBaseModel::reset() {
  for (QPair<shared_ptr<QwtPlotCurve>, shared_ptr<plugins::TimeSeries>> pair : curves.values()) {
    pair.first->detach();
  }

  curves.clear();
  registeredCurves.clear();

  maxXValue   = 50;
  fitToScreen = true;
  replot();
}

void QwtBaseModel::showContextMenu(const QPoint &point) {
  (void)point;
  QMenu menu(plot);

  QList<QAction *> actions;
  for (QPair<shared_ptr<QwtPlotCurve>, shared_ptr<plugins::TimeSeries>> pair : curves.values()) {
    QAction *action = new QAction(plot);

    action->setText(pair.first->title().text());
    action->setData(pair.first->title().text());
    action->setCheckable(true);
    action->setChecked(plot->itemList(QwtPlotItem::Rtti_PlotCurve).contains(pair.first.get()));
    connect(action, SIGNAL(changed()), this, SLOT(enablePlot()));

    actions.append(action);
  }

  for (PlotCreator::PlotCreatorData data : registeredCurves) {
    QAction *action = new QAction(plot);

    action->setText(createStringIdentifier(data));
    action->setData(createStringIdentifier(data));
    action->setDisabled(true);
    action->setCheckable(true);
    action->setChecked(true);
    connect(action, SIGNAL(changed()), this, SLOT(enablePlot()));

    actions.append(action);
  }

  if (actions.isEmpty()) {
    QAction *action = new QAction("No Plot", plot);
    action->setEnabled(false);
    actions.append(action);
  }

  menu.addActions(actions);
  menu.exec();
  actions.clear();
}

void QwtBaseModel::enablePlot() {
  QAction *act = dynamic_cast<QAction *>(sender());

  shared_ptr<QwtPlotCurve> curve = curves.value(act->data().toString()).first;
  if (curve == nullptr) {
    return;
  }

  bool checked = act->isChecked();
  if (checked)
    curve->attach(plot);
  else
    curve->detach();
  replot();
}

void QwtBaseModel::updatePlotList() {
  SettingsProfileItem::Config config = window->getSettingsWin()->getConfig();
  registeredCurves                   = config.plots;
}

bool QwtBaseModel::needUpdateAlways() { return true; }

void QwtBaseModel::setFitToPlot(bool fit) { fitToScreen = fit; }
