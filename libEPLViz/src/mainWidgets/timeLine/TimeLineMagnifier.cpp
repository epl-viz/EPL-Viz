#pragma GCC diagnostic warning "-Wfloat-equal"

#include "TimeLineMagnifier.hpp"

#include "qwt_plot.h"

TimeLineMagnifier::TimeLineMagnifier(uint32_t *maxVal, QWidget *canvas) : QwtPlotMagnifier(canvas) { max = maxVal; }
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
};
