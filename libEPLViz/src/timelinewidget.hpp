#ifndef TIMELINEWIDGET_HPP
#define TIMELINEWIDGET_HPP

#include <QDockWidget>

class TimelineWidget : public QDockWidget {
  Q_OBJECT

 public:
  TimelineWidget(QWidget *parent = nullptr);
  ~TimelineWidget()              = default;
};

#endif // TIMELINEWIDGET_HPP
