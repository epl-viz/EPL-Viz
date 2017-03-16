#pragma once

#include <QWidget>
#include <qwt_plot_magnifier.h>

class TimeLineMagnifier : public QwtPlotMagnifier {
 private:
  uint32_t *max;

 public:
  TimeLineMagnifier(uint32_t *maxVal, QWidget *canvas);

  void rescale(double factor) override;
};
