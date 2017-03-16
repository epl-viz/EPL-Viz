#pragma once
#include <QDialog>
#include <condition_variable>
#include <mutex>

namespace Ui {
class PlotCreator;
}

namespace EPL_Viz {

class PlotCreator : public QDialog {
  Q_OBJECT
 public:
  struct PlotCreatorData {
    bool isOK          = false;
    bool defaultODPlot = true;

    uint8_t  node     = UINT8_MAX;
    uint16_t index    = UINT16_MAX;
    uint8_t  subIndex = UINT8_MAX;

    std::string csName = "";
  };

 private:
  Ui::PlotCreator *       ui;
  std::mutex              waitMutex;
  std::condition_variable condVar;

  PlotCreatorData getData();
  void            wait();

 public:
  explicit PlotCreator(QWidget *parent = 0);
  ~PlotCreator();

  static PlotCreatorData getNewPlot();
};
}
