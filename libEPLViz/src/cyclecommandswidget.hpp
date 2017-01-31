#ifndef CYCLECOMMANDSWIDGET_HPP
#define CYCLECOMMANDSWIDGET_HPP

#include <QDockWidget>

class CycleCommandsWidget : public QDockWidget {
  Q_OBJECT

 public:
  CycleCommandsWidget(QWidget *parent = nullptr);
  ~CycleCommandsWidget()              = default;
};

#endif // CYCLECOMMANDSWIDGET_HPP
