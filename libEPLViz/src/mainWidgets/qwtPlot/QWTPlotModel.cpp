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
 * \file QWTPlotModel.cpp
 */

#include "QWTPlotModel.hpp"
#include "MainWindow.hpp"
#include "QPointF"

using namespace EPL_Viz;
using namespace EPL_DataCollect;

QWTPlotModel::QWTPlotModel(MainWindow *win, QwtPlot *widget) : QwtBaseModel(win, widget) {
  (void)win;
  widget->setAxisAutoScale(QwtPlot::xBottom, true);
}

QWTPlotModel::~QWTPlotModel() {}

void QWTPlotModel::createPlot(uint8_t     nodeID,
                              uint16_t    index,
                              uint16_t    subIndex,
                              std::string cs,
                              QColor      color,
                              bool        usedInTime,
                              bool        usedInPlot) {
  QwtBaseModel::createPlot(nodeID, index, subIndex, cs, QwtPlot::xBottom, color, usedInTime, usedInPlot);
}

void QWTPlotModel::changeArea(QRectF rect) {
  if (static_cast<int>(rect.left()) != static_cast<int>(rect.right())) {
    plot->setAxisScale(QwtPlot::Axis::xBottom, rect.left(), rect.right());
    plot->replot();
  }
}

void QWTPlotModel::fitToPlot() {
  plot->setAxisScale(QwtPlot::Axis::xBottom, 0, static_cast<double>(calcXMaximum()));
  plot->replot();
}
