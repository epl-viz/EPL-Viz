#ifndef INTERFACEPICKER_HPP
#define INTERFACEPICKER_HPP

#include <QDialog>

namespace Ui {
class InterfacePicker;
}
namespace EPL_Viz {
class InterfacePicker : public QDialog {
  Q_OBJECT

 public:
  explicit InterfacePicker(QWidget *parent = 0);
  ~InterfacePicker();

 private:
  Ui::InterfacePicker *ui;
};
}

#endif // INTERFACEPICKER_HPP
