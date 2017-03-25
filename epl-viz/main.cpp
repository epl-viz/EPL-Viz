#include "EPLVizDefines.hpp"
#include "Init.hpp"
#include "MainWindow.hpp"
#include "PythonInit.hpp"
#include <QApplication>

using namespace EPL_DataCollect;
using namespace EPL_DataCollect::constants;
using namespace EPL_Viz;
using namespace EPL_Viz::constants;
using namespace std;

int main(int argc, char *argv[]) {
  std::string plDir         = EPL_DC_WS_PLUGINS_DIR;
  std::string installPrefix = EPL_DC_INSTALL_PREFIX;

  if (EPL_VIZ_OVERRIDE_INSTALL_PREFIX) {
    installPrefix = "./usr";
    plDir.replace(0, EPL_DC_INSTALL_PREFIX.length(), installPrefix);
  }

  Init                init(plDir);
  plugins::PythonInit pyInit;
  if (!init.getIsOK()) {
    return 1;
  }

  pyInit.addPath(installPrefix + "/lib/eplviz");

  QApplication a(argc, argv);
  MainWindow   w;
  w.show();

  return a.exec();
}
