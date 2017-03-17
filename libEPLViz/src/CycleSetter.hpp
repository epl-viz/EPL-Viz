#pragma once

#include "CSViewFilters.hpp"
#include <QWidget>
#include <QWidgetAction>
#include <chrono>

namespace Ui {
class CycleSetter;
}

namespace EPL_Viz {

class MainWindow;

class CycleSetter : public QWidget {
  Q_OBJECT

 private:
  Ui::CycleSetter *                     ui;
  MainWindow *                          mw;
  bool                                  pointerInWindow = false;
  std::chrono::system_clock::time_point leaveTP         = std::chrono::system_clock::now();

 public:
  explicit CycleSetter(QWidget *parent, MainWindow *main);
  ~CycleSetter();

  uint32_t value();
  void setValue(uint32_t val);

  void clearFilters();
  void setFilters(std::vector<EPL_DataCollect::CSViewFilters::Filter> &f);
  std::string getCurrentFilter();

 protected:
  void enterEvent(QEvent *) override;
  void leaveEvent(QEvent *) override;

 public slots:
  void changeCycle();
};


class CycleSetterAction : public QWidgetAction {
  Q_OBJECT
 private:
  CycleSetter *cs = nullptr;
  MainWindow * mw;

 protected:
  QWidget *createWidget(QWidget *parent) override;

 public:
  explicit CycleSetterAction(QWidget *parent, MainWindow *main);
  ~CycleSetterAction();

  CycleSetter *getWidget() noexcept { return cs; }
};
}
