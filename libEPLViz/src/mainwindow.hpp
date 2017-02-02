#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 private:
  Ui::MainWindow *ui;

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 public slots:
  void mySetFullscreen(bool makeFullscreen);
  void openPluginEditor();
  void openInterfacePicker();
  void save();
  void saveAs();
  void open();
};

#endif // MAINWINDOW_H
