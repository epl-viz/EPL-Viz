#include "Statistics.hpp"
#include "CaptureInstance.hpp"
#include "MainWindow.hpp"
#include "ui_Statistics.h"

using namespace EPL_Viz;
using namespace EPL_DataCollect;

Statistics::Statistics(MainWindow *parent) : QDialog(parent), ui(new Ui::Statistics), mw(parent) {
  ui->setupUi(this);
  refresh();
}

Statistics::~Statistics() { delete ui; }

void Statistics::refresh() {
  CaptureInstance *ci    = mw->getCaptureInstance();
  auto             stats = ci->getCycleBuilder()->getStats();
  ui->List->clear();

  ui->List->addTopLevelItem(new QTreeWidgetItem({"Cycle count", std::to_string(stats.cycleCount).c_str()}));
  ui->List->addTopLevelItem(new QTreeWidgetItem({"Packet count", std::to_string(stats.packetCount).c_str()}));
  ui->List->addTopLevelItem(new QTreeWidgetItem({"Events count", std::to_string(stats.eventsCount).c_str()}));
  ui->List->addTopLevelItem(
        new QTreeWidgetItem({"Total time", (std::to_string(stats.totalTime.count()) + "ns").c_str()}));
  ui->List->addTopLevelItem(new QTreeWidgetItem(
        {"Average Cycle time", (std::to_string(stats.totalTime.count() / stats.cycleCount) + "ns").c_str()}));
}
