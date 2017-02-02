#ifndef CYCLEVIEWERWIDGET_HPP
#define CYCLEVIEWERWIDGET_HPP

#include <QDockWidget>

class CycleViewerWidget : public QDockWidget {
  Q_OBJECT

 public:
  CycleViewerWidget(QWidget *parent = nullptr);
  ~CycleViewerWidget()              = default;
};

#endif // CYCLEVIEWERWIDGET_HPP
