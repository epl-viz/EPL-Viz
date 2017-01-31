#ifndef PLUGINLISTWIDGET_HPP
#define PLUGINLISTWIDGET_HPP

#include <QDockWidget>

class PluginListWidget : public QDockWidget {
  Q_OBJECT

 public:
  PluginListWidget(QWidget *parent = nullptr);
  ~PluginListWidget()              = default;
};

#endif // PLUGINLISTWIDGET_HPP
