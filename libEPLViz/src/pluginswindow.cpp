#include "pluginswindow.hpp"
#include "ui_pluginswindow.h"

PluginsWindow::PluginsWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::PluginsWindow) { ui->setupUi(this); }

PluginsWindow::~PluginsWindow() { delete ui; }

void PluginsWindow::save() {
  // TODO
}
void PluginsWindow::saveAs() {
  // TODO
}
