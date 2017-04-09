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
  CaptureInstance *ci      = mw->getCaptureInstance();
  auto             stats   = ci->getCycleBuilder()->getStats();
  auto             ihStats = ci->getInputHandler()->getStats();
  ui->List->clear();

  // Make sure not to devide by 0
  if (stats.cycleCount == 0)
    stats.cycleCount = 1;

  if (ihStats.packetsParsed == 0)
    ihStats.packetsParsed = 1;

  if (ihStats.cyclesParsed == 0)
    ihStats.cyclesParsed = 1;

  QList<QTreeWidgetItem *> items;
  QTreeWidgetItem *        pItem = nullptr;
  items.append(new QTreeWidgetItem({"Cycle count", std::to_string(stats.cycleCount).c_str()}));
  items.append(new QTreeWidgetItem({"Cycle count", std::to_string(stats.cycleCount).c_str()}));
  items.append(new QTreeWidgetItem({"Packet count", std::to_string(stats.packetCount).c_str()}));
  items.append(new QTreeWidgetItem({"Events count", std::to_string(stats.eventsCount).c_str()}));
  items.append(new QTreeWidgetItem({"Packet parsed", std::to_string(ihStats.packetsParsed).c_str()}));
  items.append(new QTreeWidgetItem({"Cycles parsed", std::to_string(ihStats.cyclesParsed).c_str()}));
  items.append(new QTreeWidgetItem({"Cycle Processing time", ""}));
  pItem = items.back();
  items.append(new QTreeWidgetItem(pItem, {"Total", (std::to_string(stats.totalTime.count()) + " ns").c_str()}));
  items.append(new QTreeWidgetItem(
        pItem, {"Average", (std::to_string(stats.totalTime.count() / stats.cycleCount) + " ns").c_str()}));

  items.append(new QTreeWidgetItem({"Time waited for packets to be parsed", ""}));
  pItem = items.back();
  items.append(
        new QTreeWidgetItem(pItem, {"Total", (std::to_string(stats.waitForPacketsTime.count()) + " ns").c_str()}));
  items.append(new QTreeWidgetItem(
        pItem, {"Average", (std::to_string(stats.waitForPacketsTime.count() / stats.cycleCount) + " ns").c_str()}));

  items.append(new QTreeWidgetItem({"Packets parsing time", ""}));
  pItem = items.back();
  items.append(
        new QTreeWidgetItem(pItem, {"Total", (std::to_string(ihStats.timePacketsParsed.count()) + " ns").c_str()}));
  items.append(new QTreeWidgetItem(
        pItem,
        {"Average", (std::to_string(ihStats.timePacketsParsed.count() / ihStats.packetsParsed) + " ns").c_str()}));

  items.append(new QTreeWidgetItem({"Packets of a Cycle parsing time", ""}));
  pItem = items.back();
  items.append(
        new QTreeWidgetItem(pItem, {"Total", (std::to_string(ihStats.timeCyclesParsed.count()) + " ns").c_str()}));
  items.append(new QTreeWidgetItem(
        pItem, {"Average", (std::to_string(ihStats.timeCyclesParsed.count() / ihStats.cyclesParsed) + " ns").c_str()}));

  ui->List->addTopLevelItems(items);
  ui->List->expandAll();
  ui->List->header()->resizeSection(0, 275);
}
