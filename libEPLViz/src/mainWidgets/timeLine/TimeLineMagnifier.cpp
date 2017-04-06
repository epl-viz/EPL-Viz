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
 * \file TimeLineMagnifier.cpp
 */

#include "TimeLineMagnifier.hpp"
#include "TimeLineModel.hpp"
#include <QWheelEvent>
using namespace EPL_Viz;

#include "qwt_plot.h"
#include <limits>

TimeLineMagnifier::TimeLineMagnifier(QScrollBar *bar, TimeLineModel *model, uint32_t *maxVal, QWidget *canvas)
    : QwtPlotMagnifier(canvas) {
  max      = maxVal;
  modelRef = model;
  sBar     = bar;
}

void TimeLineMagnifier::rescale(double factor) {
  qDebug() << "Simple rescale called";
  TimeLineMagnifier::rescale(factor, plot()->canvas()->size().width() / 2);
}

/**
 * @brief TimeLineMagnifier::rescale Copied from qwt and changed to stop at boundries
 * @param factor Factor
 */
void TimeLineMagnifier::rescale(double factor, int x) {
  QwtPlot *plt = plot();
  if (plt == NULL)
    return;

  factor = qAbs(factor);

  if (std::fabs(factor - 1) <= std::numeric_limits<double>::epsilon() ||
      factor <= std::numeric_limits<double>::epsilon())
    return;

  bool doReplot = false;

  const bool autoReplot = plt->autoReplot();
  plt->setAutoReplot(false);

  for (int axisId = 0; axisId < QwtPlot::axisCnt; axisId++) {
    if (isAxisEnabled(axisId)) {
      const QwtScaleMap scaleMap = plt->canvasMap(axisId);

      double v1 = scaleMap.s1();
      double v2 = scaleMap.s2();

      if (scaleMap.transformation()) {
        // the coordinate system of the paint device is always linear

        v1 = scaleMap.transform(v1); // scaleMap.p1()
        v2 = scaleMap.transform(v2); // scaleMap.p2()
      }

      const double width_2 = (v2 - v1) * factor;
      double       relPos;
      if (axisId == QwtPlot::xTop)
        relPos = static_cast<double>(x) / plt->canvas()->size().width();
      else
        relPos            = .5;
      const double center = relPos * (v1 + v2);

      v1 = center - width_2 * (1 - relPos);
      v2 = center + width_2 * (relPos);

      if (scaleMap.transformation()) {
        v1 = scaleMap.invTransform(v1);
        v2 = scaleMap.invTransform(v2);
      }

      if (std::abs(v1 - v2) < 10)
        continue;
      if (v1 < 0)
        v1 = 0;
      if (v2 > *max)
        v2 = *max;

      if (axisId == QwtPlot::xTop) {
        sBar->setValue(static_cast<int>(round(v1)));
        sBar->setPageStep(static_cast<int>(round(v2 - v1)));
        modelRef->setFitToPlot(false);
      }

      plt->setAxisScale(axisId, v1, v2);
      doReplot = true;
    }
  }

  plt->setAutoReplot(autoReplot);

  if (doReplot)
    modelRef->replot();
};

void TimeLineMagnifier::widgetWheelEvent(QWheelEvent *wheelEvent) {
  double factor;
  if (wheelEvent->angleDelta().y() > 0)
    factor = wheelFactor();
  else if (wheelEvent->angleDelta().y() < 0)
    factor = 1 / wheelFactor();
  else
    return;

  int x = wheelEvent->pos().x();

  TimeLineMagnifier::rescale(factor, x);
}
