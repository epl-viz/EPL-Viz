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
 * \file timelinewidget.cpp
 */
#include "timelinewidget.hpp"
#include <QDebug>
#include <QEvent>
#include <qwt_event_pattern.h>
#include <qwt_picker_machine.h>
#include <qwt_plot.h>
#include <stdio.h>

TimelineWidget::TimelineWidget(QWidget *parent) : QDockWidget(parent) {}

TimelineWidget::~TimelineWidget() { delete (picker); }

bool TimelineWidget::event(QEvent *event) {
  // Only handling Polish events, call parent in any other case
  if (event->type() == QEvent::Polish) {
    qDebug() << "Polish event thrown";
    QwtPlot *plot = this->findChild<QwtPlot *>("qwtPlotTimeline");
    // Event gets thrown multiple times, even when not all children have been created
    if (plot) {
      // Configure axis
      plot->enableAxis(QwtPlot::yLeft, false);
      plot->enableAxis(QwtPlot::xBottom, false);
      plot->enableAxis(QwtPlot::xTop, true);

      // Configure PlotPicker
      picker = new QwtPlotPicker(plot->canvas());
      picker->setStateMachine(new QwtPickerClickPointMachine());
      picker->setMousePattern(QwtEventPattern::MousePatternCode::MouseSelect1, Qt::MouseButton::LeftButton);
      connect(picker, SIGNAL(selected(QPointF)), this, SLOT(pointSelected(QPointF)));

    } else {
      qDebug() << "PlotTimeline not found, this is ok";
    }
  } else {
    return QWidget::event(event);
  }
  return true;
}

void TimelineWidget::pointSelected(const QPointF &pa) {
  qDebug() << "Clicked on Timeline at point " << pa.x();
  // TODO (try to) jump to selected time
}
