#ifndef PACKETHISTORYWIDGET_HPP
#define PACKETHISTORYWIDGET_HPP

#include <QDockWidget>

class PacketHistoryWidget : public QDockWidget {
  Q_OBJECT

 public:
  PacketHistoryWidget(QWidget *parent = nullptr);
  ~PacketHistoryWidget()              = default;
};

#endif // PACKETHISTORYWIDGET_HPP
