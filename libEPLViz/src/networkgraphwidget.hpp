#ifndef NETWORKGRAPHWIDGET_HPP
#define NETWORKGRAPHWIDGET_HPP

#include <QDockWidget>

class NetworkGraphWidget : public QDockWidget {
  Q_OBJECT

 public:
  NetworkGraphWidget(QWidget *parent = nullptr);
  ~NetworkGraphWidget()              = default;
};

#endif // NETWORKGRAPHWIDGET_HPP
