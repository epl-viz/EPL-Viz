#include "CycleSetter.hpp"
#include "MainWindow.hpp"

using namespace EPL_Viz;
using namespace std;
using namespace std::chrono;

#include "ui_CycleSetter.h"

CycleSetter::CycleSetter(QWidget *parent, MainWindow *main) : QWidget(parent), ui(new Ui::CycleSetter), mw(main) {
  ui->setupUi(this);
  ui->counter->setMinimum(0);
  ui->counter->setMaximum(0);
}
CycleSetter::~CycleSetter() { delete ui; }


uint32_t CycleSetter::value() { return static_cast<uint32_t>(ui->counter->value()); }

void CycleSetter::updateCounterRange() {
  uint32_t maxCycle = mw->getCaptureInstance()->getCycleBuilder()->getStats().cycleCount;
  int      val      = static_cast<int>(maxCycle);
  if (val < ui->counter->maximum())
    lastValueSet = maxCycle == UINT32_MAX ? 0 : maxCycle;

  ui->counter->setMaximum(val < 0 ? 0 : val);
}

void CycleSetter::setValue(uint32_t val) {
  if (val == UINT32_MAX || val == value())
    return;

  if (static_cast<int>(val) > ui->counter->maximum())
    ui->counter->setMaximum(static_cast<int>(val));

  if (val > value()) {
    ui->skipF->setEnabled(true);
    ui->seekF->setEnabled(true);
  }

  if (!ui->widget->isMouseInWidget()) {
    if (system_clock::now() - leaveTP > seconds(1)) {
      lastValueSet = val;
      ui->counter->setValue(static_cast<int>(val));
    }
  }
}

void MouseTrackingWidget::enterEvent(QEvent *) { pointerInWindow = true; }
void MouseTrackingWidget::leaveEvent(QEvent *) {
  pointerInWindow = false;
  parent->resetTimer();
  parent->checkButtons();
}


void CycleSetter::checkButtons() {
  uint32_t val = value();
  updateCounterRange();
  if (val <= 0) {
    ui->skipB->setEnabled(false);
    ui->seekB->setEnabled(false);
  } else {
    ui->skipB->setEnabled(true);
    ui->seekB->setEnabled(true);
  }

  if (static_cast<int>(val) >= ui->counter->maximum()) {
    ui->skipF->setEnabled(false);
    ui->seekF->setEnabled(false);
  } else {
    ui->skipF->setEnabled(true);
    ui->seekF->setEnabled(true);
  }
}

void CycleSetter::changeCycle() {
  uint32_t val = value();
  lastValueSet = UINT32_MAX;
  mw->changeCycle(val);
  updateCounterRange();
  checkButtons();
}

void CycleSetter::changeTempCycle(int) {
  updateCounterRange();
  if (!mw->getSettingsWin()->getConfig().immidiateCycleChange)
    return;

  uint32_t val = value();

  // only change cycle when the user made an imput (and not QT)
  if (val == lastValueSet)
    return;

  qDebug() << "SET CYCLE: " << val << " -- " << lastValueSet;

  lastValueSet = UINT32_MAX;

  mw->changeCycle(val);
  checkButtons();
}

void CycleSetter::skipF() {
  updateCounterRange();
  mw->changeCycle(UINT32_MAX);
  mw->fitTimeline();
  ui->skipF->setEnabled(false);
  ui->seekF->setEnabled(false);
  ui->skipB->setEnabled(true);
  ui->seekB->setEnabled(true);
}

void CycleSetter::skipB() {
  updateCounterRange();
  mw->changeCycle(0);
  mw->fitTimeline();
  ui->skipB->setEnabled(false);
  ui->seekB->setEnabled(false);
  ui->skipF->setEnabled(true);
  ui->seekF->setEnabled(true);
}

void CycleSetter::seekF() {
  updateCounterRange();
  uint32_t newVal = value() + 1;
  ui->counter->setValue(static_cast<int>(newVal));
  mw->changeCycle(newVal);
  if (static_cast<int>(newVal) == ui->counter->maximum()) {
    ui->skipF->setEnabled(false);
    ui->seekF->setEnabled(false);
  }

  ui->skipB->setEnabled(true);
  ui->seekB->setEnabled(true);
}

void CycleSetter::seekB() {
  updateCounterRange();
  uint32_t newVal = value() - 1;
  ui->counter->setValue(static_cast<int>(newVal));
  mw->changeCycle(newVal);
  if (newVal == 0) {
    ui->skipB->setEnabled(false);
    ui->seekB->setEnabled(false);
  }

  ui->skipF->setEnabled(true);
  ui->seekF->setEnabled(true);
}

void CycleSetter::clearFilters() { ui->filters->clear(); }

void CycleSetter::setFilters(std::vector<EPL_DataCollect::CSViewFilters::Filter> &f) {
  if (ui->filters->count() != 0 || f.empty())
    return;

  ui->filters->addItem("All", QVariant("All STR"));

  for (auto &i : f) {
    ui->filters->addItem(i.getName().c_str(), QVariant(QString(i.getName().c_str())));
  }
}

std::string CycleSetter::getCurrentFilter() { return ui->filters->currentData().toString().toStdString(); }


CycleSetterAction::CycleSetterAction(QWidget *parent, MainWindow *main) : QWidgetAction(parent), mw(main) {}
CycleSetterAction::~CycleSetterAction() {
  if (cs)
    delete cs;
}

QWidget *CycleSetterAction::createWidget(QWidget *parent) {
  cs = new CycleSetter(parent, mw);
  return cs;
}

void CycleSetter::fitTime() { mw->fitTimeline(); }
