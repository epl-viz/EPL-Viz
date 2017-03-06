#ifndef SETTINGSWINDOWS_HPP
#define SETTINGSWINDOWS_HPP

#include <QMainWindow>

namespace Ui {
class SettingsWindows;
}

class SettingsWindows : public QMainWindow {
  Q_OBJECT

 public:
  explicit SettingsWindows(QWidget *parent = 0);
  ~SettingsWindows();

 private:
  Ui::SettingsWindows *ui;
};

#endif // SETTINGSWINDOWS_HPP
