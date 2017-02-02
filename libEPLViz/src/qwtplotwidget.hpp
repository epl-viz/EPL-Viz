#ifndef QWTPLOTWIDGET_HPP
#define QWTPLOTWIDGET_HPP

#include <QDockWidget>

class QWTPlotWidget : public QDockWidget {
  Q_OBJECT

 public:
  QWTPlotWidget(QWidget *parent = nullptr);
  ~QWTPlotWidget()              = default;
};

#endif // QWTPLOTWIDGET_HPP
