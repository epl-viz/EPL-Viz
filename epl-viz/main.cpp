#include "Init.hpp"
#include "mainwindow.hpp"
#include <QApplication>

int main(int argc, char *argv[]) {
  EPL_DataCollect::Init init;
  if (!init.getIsOK()) {
    return 1;
  }
  QApplication a(argc, argv);
  MainWindow   w;
  w.show();

  return a.exec();
}
