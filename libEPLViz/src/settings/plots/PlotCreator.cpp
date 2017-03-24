#include "PlotCreator.hpp"
#include "MainWindow.hpp"
#include <QColorDialog>

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

  auto pal = ui->COL_PLOT->palette();
  d.color  = pal.color(QPalette::Window);

  return d;
}


PlotCreator::PlotCreatorData PlotCreator::getNewPlot(PlotCreatorData *oldData) {
  PlotCreator plt(nullptr);

  if (oldData) {
    plt.ui->toolBox->setCurrentIndex(oldData->defaultODPlot ? 0 : 1);
    plt.ui->csID->setText(oldData->csName.c_str());
    plt.ui->node->setValue(static_cast<int>(oldData->node));
    plt.ui->index->setValue(static_cast<int>(oldData->index));
    plt.ui->subIndex->setValue(static_cast<int>(oldData->subIndex));
    plt.ui->AddToTimeLine->setCheckState(oldData->addToTimeLine ? Qt::Checked : Qt::Unchecked);
    plt.ui->AddToPlot->setCheckState(oldData->addToPlot ? Qt::Checked : Qt::Unchecked);
    auto pal = plt.palette();
    pal.setColor(QPalette::Window, oldData->color);
    plt.ui->COL_PLOT->setPalette(pal);
  }

  int  res = plt.exec();
  auto d   = plt.getData();
  d.isOK   = (res == QDialog::Accepted);
  return d;
}

void PlotCreator::changeColor() {
  QPalette pal = ui->COL_PLOT->palette();

  QColor newColor = QColorDialog::getColor(pal.color(QPalette::Window), this, "Select color");
  if (!newColor.isValid())
    return;

  pal.setColor(QPalette::Window, newColor);
  ui->COL_PLOT->setPalette(pal);
}
