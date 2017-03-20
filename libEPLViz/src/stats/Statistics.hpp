#pragma once

#include <QDialog>

namespace Ui {
class Statistics;
}

namespace EPL_Viz {

class MainWindow;

class Statistics : public QDialog {
  Q_OBJECT

 private:
  Ui::Statistics *ui;
  MainWindow *    mw;

 public:
  explicit Statistics(MainWindow *parent);
  ~Statistics();

 public slots:
  void refresh();
};
}
