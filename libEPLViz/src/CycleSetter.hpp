#pragma once

#include <QWidget>
#include <QWidgetAction>

namespace Ui {
class CycleSetter;
}

namespace EPL_Viz {

class MainWindow;

class CycleSetter : public QWidget {
  Q_OBJECT

 private:
  Ui::CycleSetter *ui;
  MainWindow *     mw;

 public:
  explicit CycleSetter(QWidget *parent, MainWindow *main);
  ~CycleSetter();

  uint32_t value();
  void setValue(uint32_t val);

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
