#include "Init.hpp"
#include "MainWindow.hpp"
#include "PythonInit.hpp"
#include <QApplication>

int main(int argc, char *argv[]) {
  EPL_DataCollect::Init                init;
  EPL_DataCollect::plugins::PythonInit pyInit;
  if (!init.getIsOK()) {
    return 1;
  }
  QApplication        a(argc, argv);
  EPL_Viz::MainWindow w;
  w.show();

  return a.exec();
}
