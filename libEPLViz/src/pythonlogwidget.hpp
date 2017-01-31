#ifndef PYTHONLOGWIDGET_HPP
#define PYTHONLOGWIDGET_HPP

#include <QDockWidget>

class PythonLogWidget : public QDockWidget {
  Q_OBJECT

 public:
  PythonLogWidget(QWidget *parent = nullptr);
  ~PythonLogWidget()              = default;
};

#endif // PYTHONLOGWIDGET_HPP
