#include "CycleSetter.hpp"
#include "MainWindow.hpp"
#include "ui_CycleSetter.h"

using namespace EPL_Viz;

CycleSetter::CycleSetter(QWidget *parent, MainWindow *main) : QWidget(parent), ui(new Ui::CycleSetter), mw(main) {
  ui->setupUi(this);
}
CycleSetter::~CycleSetter() { delete ui; }


uint32_t CycleSetter::value() { return static_cast<uint32_t>(ui->counter->value()); }
void CycleSetter::setValue(uint32_t val) { ui->counter->setValue(static_cast<int>(val)); }

void CycleSetter::changeCycle() { mw->changeCycle(value()); }


CycleSetterAction::CycleSetterAction(QWidget *parent, MainWindow *main) : QWidgetAction(parent), mw(main) {}
CycleSetterAction::~CycleSetterAction() {
  if (cs)
    delete cs;
}

QWidget *CycleSetterAction::createWidget(QWidget *parent) {
  cs = new CycleSetter(parent, mw);
  return cs;
}
