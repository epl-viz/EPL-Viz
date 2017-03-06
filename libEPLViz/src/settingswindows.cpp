#include "settingswindows.hpp"
#include "ui_settingswindows.h"

SettingsWindows::SettingsWindows(QWidget *parent) : QMainWindow(parent), ui(new Ui::SettingsWindows) {
  ui->setupUi(this);
}

SettingsWindows::~SettingsWindows() { delete ui; }
