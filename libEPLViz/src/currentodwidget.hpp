#ifndef CURRENTODWIDGET_HPP
#define CURRENTODWIDGET_HPP

#include <QDockWidget>

class CurrentODWidget : public QDockWidget {
  Q_OBJECT

 public:
  CurrentODWidget(QWidget *parent = nullptr);
  ~CurrentODWidget()              = default;
};

#endif // CURRENTODWIDGET_HPP
