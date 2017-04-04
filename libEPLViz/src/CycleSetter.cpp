#include "CycleSetter.hpp"
#include "MainWindow.hpp"

using namespace EPL_Viz;
using namespace std;
using namespace std::chrono;

#include "ui_CycleSetter.h"

CycleSetter::CycleSetter(QWidget *parent, MainWindow *main) : QWidget(parent), ui(new Ui::CycleSetter), mw(main) {
  ui->setupUi(this);
}
CycleSetter::~CycleSetter() { delete ui; }


uint32_t CycleSetter::value() { return static_cast<uint32_t>(ui->counter->value()); }

void CycleSetter::setValue(uint32_t val) {
  if (val > value()) {
    ui->skipF->setEnabled(true);
    ui->seekF->setEnabled(true);
  }

  if (!ui->widget->isMouseInWidget()) {
    if (system_clock::now() - leaveTP > seconds(1)) {
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
  if (val <= 0) {
    ui->skipB->setEnabled(false);
    ui->seekB->setEnabled(false);
  } else {
    ui->skipB->setEnabled(true);
    ui->seekB->setEnabled(true);
  }

  if (val >= mw->getMaxCycle()) {
    ui->skipF->setEnabled(false);
    ui->seekF->setEnabled(false);
  } else {
    ui->skipF->setEnabled(true);
    ui->seekF->setEnabled(true);
  }
}

void CycleSetter::changeCycle() {
  uint32_t val = value();
  mw->changeCycle(val);
  checkButtons();
}

void CycleSetter::skipF() {
  mw->changeCycle(UINT32_MAX);
  ui->skipF->setEnabled(false);
  ui->seekF->setEnabled(false);
  ui->skipB->setEnabled(true);
  ui->seekB->setEnabled(true);
}

void CycleSetter::skipB() {
  mw->changeCycle(0);
  ui->skipB->setEnabled(false);
  ui->seekB->setEnabled(false);
  ui->skipF->setEnabled(true);
  ui->seekF->setEnabled(true);
}

void CycleSetter::seekF() {
  uint32_t newVal = value() + 1;
  ui->counter->setValue(static_cast<int>(newVal));
  mw->changeCycle(newVal);
  if (newVal == mw->getMaxCycle()) {
    ui->skipF->setEnabled(false);
    ui->seekF->setEnabled(false);
  }

  ui->skipB->setEnabled(true);
  ui->seekB->setEnabled(true);
}

void CycleSetter::seekB() {
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
