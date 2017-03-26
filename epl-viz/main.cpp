#include "EPLVizDefines.hpp"
#include "Init.hpp"
#include "MainWindow.hpp"
#include "PythonInit.hpp"
#include <QApplication>
#include <QMessageBox>
#include <stdlib.h>

using namespace EPL_DataCollect;
using namespace EPL_DataCollect::constants;
using namespace EPL_Viz;
using namespace EPL_Viz::constants;
using namespace std;

int main(int argc, char *argv[]) {
  std::string plDir         = EPL_DC_WS_PLUGINS_DIR;
  std::string installPrefix = EPL_DC_INSTALL_PREFIX;

  char *appImageDir = getenv("APPDIR");
  if (appImageDir) {
    installPrefix = appImageDir;
    installPrefix += "/usr";
    plDir.replace(0, EPL_DC_INSTALL_PREFIX.length(), installPrefix);
  }

  Init                init(plDir, installPrefix + "/lib");
  plugins::PythonInit pyInit;

  if (init.getIsOK() != Init::OK) {
    QApplication a(argc, argv);
    QMessageBox::critical(nullptr,
                          "Failed to initialize",
                          QString("Failed to initialize the EPL DataCollect backend\n  -- Error Code: ") +
                                EPLEnum2Str::toStr(init.getIsOK()).c_str());
    return 1;
  }
  pyInit.addPath(installPrefix + "/lib/eplViz");

  QApplication a(argc, argv);
  MainWindow   w;
  w.show();

  return a.exec();
}
