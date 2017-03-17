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
#pragma GCC diagnostic warning "-Wfloat-equal"

#include "TimeLineMagnifier.hpp"
#include "TimeLineModel.hpp"
using namespace EPL_Viz;

#include "qwt_plot.h"

TimeLineMagnifier::TimeLineMagnifier(TimeLineModel *model, uint32_t *maxVal, QWidget *canvas)
    : QwtPlotMagnifier(canvas) {
  max      = maxVal;
  modelRef = model;
}

/**
 * @brief TimeLineMagnifier::rescale Copied from qwt and changed to stop at boundries
 * @param factor Factor
 */
void TimeLineMagnifier::rescale(double factor) {
  QwtPlot *plt = plot();
  if (plt == NULL)
    return;

  factor = qAbs(factor);
  if (factor == 1.0 || factor == 0.0)
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

      const double center  = 0.5 * (v1 + v2);
      const double width_2 = 0.5 * (v2 - v1) * factor;

      v1 = center - width_2;
      v2 = center + width_2;

      if (scaleMap.transformation()) {
        v1 = scaleMap.invTransform(v1);
        v2 = scaleMap.invTransform(v2);
      }

      if (v1 < 0)
        v1 = 0;
      if (v2 > *max)
        v2 = *max;


      plt->setAxisScale(axisId, v1, v2);
      doReplot = true;
    }
  }

  plt->setAutoReplot(autoReplot);

  if (doReplot)
    plt->replot();

  modelRef->replot();
};
