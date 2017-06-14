#include "EPLVizDefines.hpp"
#include "Init.hpp"
#include "MainWindow.hpp"
#include "PythonInit.hpp"
#include <QApplication>
#include <QMessageBox>

#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32)
#include <stdlib.h>
#endif

using namespace EPL_DataCollect;
using namespace EPL_DataCollect::constants;
using namespace EPL_Viz;
using namespace EPL_Viz::constants;
using namespace std;
using namespace std::chrono;

int main(int argc, char *argv[]) {
  std::string installPrefix = EPL_DC_INSTALL_PREFIX;

  srand(static_cast<unsigned int>(system_clock::to_time_t(system_clock::now())));

#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32)
  char *appImageDir = getenv("APPDIR");
  if (appImageDir) {
    installPrefix = appImageDir;
    installPrefix += "/usr";
  }
#endif

  Init                init;
  plugins::PythonInit pyInit(argc, argv);

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
