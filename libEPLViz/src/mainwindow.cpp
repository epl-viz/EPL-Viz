/* Copyright (c) 2017, EPL-Vizards
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the EPL-Vizards nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL EPL-Vizards BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*!
 * \file mainwindow.cpp
 */
#include "mainwindow.hpp"
#include "interfacepicker.hpp"
#include "pluginswindow.hpp"
#include "ui_mainwindow.h"
#include <QAction>
#include <QLabel>
#include <QToolButton>
#include <iostream>
#include <vector>
using namespace EPL_Viz;

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
  fixQToolButtons(btns, ui->toolBar);

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

void MainWindow::fixQToolButtons(std::vector<QToolButton *> &btns) {
  for (auto i : btns) {
    if (!i)
      continue;
    i->setPopupMode(QToolButton::InstantPopup);
  }
}

void MainWindow::fixQToolButtons(std::vector<QAction *> &actions, QToolBar *bar) {
  std::vector<QToolButton *> btns;
  btns.reserve(actions.size());
  for (auto i : actions) {
    if (!i)
      continue;
    btns.emplace_back(dynamic_cast<QToolButton *>(bar->widgetForAction(i)));
  }
  fixQToolButtons(btns);
}

bool MainWindow::changeTime(double t) {
  // TODO convert to cycle
  (void)t;
  return false;
}

bool MainWindow::changeCycle(int cycle) {
  // TODO Maybe check upper bounds? Or just ignore it and let the update method deal with it.
  //      Or if >max just use the last cycle.
  if (cycle < 0)
    return false;
  if (machineState == GUIState::STOPPED) {
    curCycle = cycle;
    return true;
  } else {
    return false;
  }
}

GUIState MainWindow::getState() { return machineState; }

bool MainWindow::startLoop() {
  if (machineState != GUIState::UNINIT)
    return false;
  // TODO create thread
  return false;
}

void MainWindow::loop() {
  while (true) {
    switch (machineState) {
      case GUIState::PAUSED:
        // TODO
        break;
      case GUIState::PLAYING:
        // TODO
        break;
      case GUIState::RECORDING:
        // TODO
        break;
      case GUIState::STOPPED:
        // TODO
        break;
      case GUIState::UNINIT:
        // TODO
        break;
    }
  }
}

void MainWindow::setFullscreen(bool makeFullscreen) {
  if (makeFullscreen) {
    showFullScreen();
  } else {
    showNormal();
  }
}

void MainWindow::openPluginEditor() {
  PluginsWindow *win = new PluginsWindow();
  win->show();
}

void MainWindow::openInterfacePicker() {
  InterfacePicker *picker = new InterfacePicker();
  picker->show();
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
