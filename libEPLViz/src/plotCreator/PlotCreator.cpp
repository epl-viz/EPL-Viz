#include "PlotCreator.hpp"
#include "MainWindow.hpp"

using namespace EPL_Viz;

#include "ui_PlotCreator.h"

PlotCreator::PlotCreator(QWidget *parent) : QDialog(parent), ui(new Ui::PlotCreator) { ui->setupUi(this); }
PlotCreator::~PlotCreator() { delete ui; }

PlotCreator::PlotCreatorData PlotCreator::getData() {
  PlotCreatorData d;

  d.defaultODPlot = ui->toolBox->currentIndex() == 0;
  d.csName        = ui->csID->text().toStdString();
  d.node          = static_cast<uint8_t>(ui->node->value());
  d.index         = static_cast<uint16_t>(ui->index->value());
  d.subIndex      = static_cast<uint8_t>(ui->subIndex->value());
  d.addToTimeLine = ui->AddToTimeLine->checkState() == Qt::Checked;
  d.addToPlot     = ui->AddToPlot->checkState() == Qt::Checked;

  return d;
}


PlotCreator::PlotCreatorData PlotCreator::getNewPlot() {
  PlotCreator plt(nullptr);
  int         res = plt.exec();
  auto        d   = plt.getData();
  d.isOK          = (res == QDialog::Accepted);
  return d;
}
