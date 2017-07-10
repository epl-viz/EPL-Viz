#include "CrashDialog.hpp"
#include "EPLVizDefines.hpp"
#include "Init.hpp"
#include "MainWindow.hpp"
#include "PythonInit.hpp"
#include "TracerHandler.hpp"
#include <QApplication>
#include <QIcon>
#include <QMessageBox>
#include <QtGlobal>

#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32)
#include <stdlib.h>
#endif

#if __cplusplus <= 201402L
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

using namespace EPL_DataCollect;
using namespace EPL_DataCollect::constants;
using namespace EPL_Viz;
using namespace EPL_Viz::constants;
using namespace std;
using namespace std::chrono;
using namespace tracer;

void handler(Tracer *t, AbstractPrinter *p, void *ctx) {
  (void)t;
  (void)ctx;
  string      outStr = p->generateString();
  CrashDialog diag(QString::fromStdString(outStr));
  diag.exec();
}

int main(int argc, char *argv[]) {
  std::string installPrefix = EPL_DC_INSTALL_PREFIX;

  srand(static_cast<unsigned int>(system_clock::to_time_t(system_clock::now())));

#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32)
  char *appImageDir = getenv("APPDIR");
  if (appImageDir) {
    installPrefix = appImageDir;
    installPrefix += "/usr";
  }
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32)
  fs::path rootDir(argv[0]);
  installPrefix = rootDir.remove_filename().remove_filename().string();
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

  auto *th              = TracerHandler::getTracer();
  auto  cfg             = th->getConfig();
  cfg.callback          = handler;
  cfg.autoPrintToStdErr = true;
  th->setConfig(cfg);

  auto          pc    = PrinterContainer::fancy();
  FancyPrinter *fP    = dynamic_cast<FancyPrinter *>(pc());
  auto          Pcfg  = fP->getConfig();
  Pcfg.shortenFiles   = true;
  Pcfg.shortenModules = true;
  fP->setConfig(Pcfg);
  fP->addSystemEntry({"Qt", qVersion()});
  fP->addSystemEntry({"EPL-Viz",
                      string("v") + to_string(EPL_VIZ_VERSION_MAJOR) + "." + to_string(EPL_VIZ_VERSION_MINOR) + "." +
                            to_string(EPL_VIZ_VERSION_SUBMINOR) + " +" + to_string(EPL_VIZ_GIT_LAST_TAG_DIFF) + " -- " +
                            EPL_VIZ_VERSION_GIT});
  fP->addSystemEntry({"EPL_DataCollect",
                      string("v") + to_string(EPL_DC_VERSION_MAJOR) + "." + to_string(EPL_DC_VERSION_MINOR) + "." +
                            to_string(EPL_DC_VERSION_SUBMINOR) + " +" + to_string(EPL_DC_GIT_LAST_TAG_DIFF) + " -- " +
                            EPL_DC_VERSION_GIT});
  th->setup(move(pc));

  if (QIcon::fromTheme("application-exit").isNull()) {
    QWidget temp;
    qDebug() << "[THEME] Using integrated theme icons";
    if (temp.palette().window().color().lightness() >= 125) {
      qDebug() << "[THEME] Using LIGHT theme icons";
      QIcon::setThemeName("Breeze_icons_Normal");
    } else {
      qDebug() << "[THEME] Using DARK theme icons";
      QIcon::setThemeName("Breeze_icons_Dark");
    };
  } else {
    qDebug() << "[THEME] Using system theme";
  }

  MainWindow w;
  w.show();

  return a.exec();
}
