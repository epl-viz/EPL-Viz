#include "mainwindow.hpp"
#include "pluginswindow.hpp"
#include "ui_mainwindow.h"
#include <QAction>
#include <QLabel>
#include <QToolButton>
#include <iostream>
#include <utils.hpp>
#include <vector>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  Q_INIT_RESOURCE(resources);

  ui->setupUi(this);
  tabifyDockWidget(ui->dockCurrent, ui->dockOD);
  tabifyDockWidget(ui->dockPlugins, ui->dockEvents);
  tabifyDockWidget(ui->dockPlugins, ui->dockNetTree);
  tabifyDockWidget(ui->dockCycle, ui->dockPLog);
  tabifyDockWidget(ui->dockCycle, ui->dockPText);
  ui->actionOD_Filter_2->setMenu(ui->menuOD_Filter);

  std::vector<QAction *> btns;
  btns.emplace_back(ui->actionOD_Filter_2);
  Utils::fixQToolButtons(btns, ui->toolBar);

  //   QPixmap pm;
  //   pm.load( ":/icons/resources/Screen_Shot.png" );
  //   QPixmap newPm = pm.scaledToHeight( 400 );
  //   ui->labelIMG->setPixmap( newPm );
  //   ui->labelIMG->resize( newPm.size() );
  //   ui->labelIMG->show();
}

MainWindow::~MainWindow() {
  Q_CLEANUP_RESOURCE(resources);
  delete ui;
}

void MainWindow::mySetFullscreen(bool makeFullscreen) {
  if (makeFullscreen) {
    showFullScreen();
  } else {
    showNormal();
  }
}

void MainWindow::myOpenPluginEditor() {
  PluginsWindow *win = new PluginsWindow();
  win->show();
}

void MainWindow::save() {
  // TODO
}
void MainWindow::saveAs() {
  // TODO
}
void MainWindow::open() {
  // TODO
}
