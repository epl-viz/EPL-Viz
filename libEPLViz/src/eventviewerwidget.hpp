#ifndef EVENTVIEWERWIDGET_HPP
#define EVENTVIEWERWIDGET_HPP

#include <QDockWidget>

class EventViewerWidget : public QDockWidget {
  Q_OBJECT

 public:
  EventViewerWidget(QWidget *parent = nullptr);
  ~EventViewerWidget()              = default;
};

#endif // EVENTVIEWERWIDGET_HPP
