#include "timelinewidget.hpp"
#include "qwt_plot.h"

TimelineWidget::TimelineWidget(QWidget *parent) : QDockWidget(parent) {
  QwtPlot plot = (QWTPlot)this->findChild("qwtPlotTimeline");
}
