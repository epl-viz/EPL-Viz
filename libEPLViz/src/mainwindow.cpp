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
#include "TimeSeriesBuilder.hpp"
#include "interfacepicker.hpp"
#include "pluginswindow.hpp"
#include "ui_mainwindow.h"
#include <memory>
using namespace EPL_Viz;
using namespace EPL_DataCollect;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  Q_INIT_RESOURCE(resources);

  machineState = GUIState::UNINIT;

  ui->setupUi(this);
  tabifyDockWidget(ui->dockCurrent, ui->dockOD);
  tabifyDockWidget(ui->dockPlugins, ui->dockEvents);
  tabifyDockWidget(ui->dockPlugins, ui->dockNetTree);
  tabifyDockWidget(ui->dockCycle, ui->dockPythonLog);
  tabifyDockWidget(ui->dockCycle, ui->dockPacketHistory);
  ui->actionOD_Filter_2->setMenu(ui->menuOD_Filter);

  std::vector<QAction *> btns;
  btns.emplace_back(ui->actionOD_Filter_2);
  fixQToolButtons(btns, ui->toolBar);

  modelThread = new ModelThread(this, &machineState, this);
  connect(modelThread, &ModelThread::resultReady, this, &MainWindow::handleResults);
  connect(modelThread, &ModelThread::finished, modelThread, &QObject::deleteLater);
  modelThread->start();

  connect(this, SIGNAL(close()), modelThread, SLOT(stop()));
}

MainWindow::~MainWindow() {
  Q_CLEANUP_RESOURCE(resources);
  destroyModels();
  delete ui;
}

void MainWindow::createModels() {
  // Create and add Models here
  // TODO disabled because of unfinished Cycle generation/getting
  // models.append(new PacketHistoryModel(this));
  // models.append(new PythonLogModel(this));
  // models.append(new QWTPlotModel(this));
  models.append(new CurrentODModel(this));
}

void MainWindow::destroyModels() {
  while (!models.isEmpty()) {
    delete models.takeFirst();
  }
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

bool MainWindow::event(QEvent *event) {
  // configure stuff
  if (event->type() == QEvent::Polish) {
    qDebug() << "Polish in main";
    // create Models
    createModels();
    captureInstance = std::make_unique<CaptureInstance>();
    // add Plugins
    captureInstance->getPluginManager()->addPlugin(std::make_shared<plugins::TimeSeriesBuilder>());
  }
  return QMainWindow::event(event);
}

int MainWindow::getCycleNum() { return curCycle; }

CaptureInstance *MainWindow::getCaptureInstance() { return captureInstance.get(); }

void MainWindow::save() {
  // TODO
}
void MainWindow::saveAs() {
  // TODO
}
void MainWindow::open() {
  // TODO
}

void MainWindow::startRecording() {
  // Set other Actions enabled/disabled
  dynamic_cast<QAction *>(sender())->setEnabled(false);
  findChild<QAction *>("actionStart_Recording")->setEnabled(true);
  machineState = GUIState::RECORDING;
}

void MainWindow::stopRecording() {
  dynamic_cast<QAction *>(sender())->setEnabled(false);
  findChild<QAction *>("actionStop_Recording")->setEnabled(true);
  machineState = GUIState::STOPPED;
void MainWindow::closeEvent(QCloseEvent *event)
{
    emit close();
    QWidget::closeEvent(event);
}

void MainWindow::handleResults(const QString &result) { qDebug() << "The result is\"" << result << "\""; }
