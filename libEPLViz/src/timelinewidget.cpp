#include "timelinewidget.hpp"
#include "qwt_plot.h"
#include <QEvent>

TimelineWidget::TimelineWidget(QWidget *parent) : QDockWidget(parent) {
}

bool QWidget::event(QEvent *event) {
  // Only handling Polish events, call parent in any other case
  if (event->type() == QEvent::Polish) {
    // Setting options of the QwtPlot
    QString name = "qwtPlotTimeline";
    QwtPlot *plot = this->findChild<QwtPlot *>(name);
    plot->enableAxis(QwtPlot::yLeft, false);
    plot->enableAxis(QwtPlot::xBottom, false);
  } else {
    return QWidget::event(event);
  }
  return true;
}
