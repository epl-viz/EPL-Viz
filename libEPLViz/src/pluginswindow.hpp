#ifndef PLUGINSWINDOW_HPP
#define PLUGINSWINDOW_HPP

#include <QMainWindow>

namespace Ui {
class PluginsWindow;
}

class PluginsWindow : public QMainWindow {
  Q_OBJECT

 public:
  explicit PluginsWindow(QWidget *parent = 0);
  ~PluginsWindow();

 private:
  Ui::PluginsWindow *ui;

 public slots:
  void save();
  void saveAs();
};

#endif // PLUGINSWINDOW_HPP
