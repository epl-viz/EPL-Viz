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
#include "CSTimeSeriesPtr.hpp"
#include "EPLVizEnum2Str.hpp"
#include "TimeSeriesBuilder.hpp"
#include "cyclecommandsmodel.hpp"
#include "interfacepicker.hpp"
#include "networkgraphmodel.hpp"
#include "oddescriptionmodel.hpp"
#include "pluginswindow.hpp"
#include "settingswindow.hpp"
#include "settingswindow.hpp"
#include "ui_mainwindow.h"
#include <memory>
#include <vector>
#include <wiretap/wtap.h>

using namespace EPL_Viz;
using namespace EPL_DataCollect;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  Q_INIT_RESOURCE(resources);

  machineState = GUIState::UNINIT;

  profileManager = new ProfileManager();

  ui->setupUi(this);
  tabifyDockWidget(ui->dockCurrent, ui->dockOD);
  tabifyDockWidget(ui->dockPlugins, ui->dockEvents);
  ui->actionOD_Filter_2->setMenu(ui->menuOD_Filter);

  std::vector<QAction *> btns;
  btns.emplace_back(ui->actionOD_Filter_2);
  fixQToolButtons(btns, ui->toolBar);

  modelThread = new ModelThread(this, &machineState, this);
  connect(modelThread, &ModelThread::resultReady, this, &MainWindow::handleResults);
  connect(modelThread, &ModelThread::finished, modelThread, &QObject::deleteLater);
  modelThread->start();

  connect(this,
          SIGNAL(recordingStarted(EPL_DataCollect::CaptureInstance *)),
          ui->pluginSelectorWidget,
          SLOT(loadPlugins(
                EPL_DataCollect::CaptureInstance *))); // Notify the pluginSelectorWidget of the start of recording
  connect(this,
          SIGNAL(recordingStarted(EPL_DataCollect::CaptureInstance *)),
          ui->eventLog,
          SLOT(start(EPL_DataCollect::CaptureInstance *))); // Notify the eventLog of the start of recording
  connect(this, SIGNAL(close()), modelThread, SLOT(stop()));

  profileManager->getDefaultProfile()->readWindowSettings(this);
  captureInstance = std::make_unique<CaptureInstance>();

  settingsWin = new SettingsWindow(this, profileManager);
  settingsWin->hide();

  ui->pluginSelectorWidget->setMainWindow(this);
}

MainWindow::~MainWindow() {
  Q_CLEANUP_RESOURCE(resources);
  destroyModels();
  delete profileManager;
  delete ui;
  delete settingsWin;
}

void MainWindow::addNode(Node *n) {
  NodeWidget * nw     = new NodeWidget(n, ui->networkGraphContents);
  QGridLayout *layout = qobject_cast<QGridLayout *>(ui->networkGraphContents->layout());

  int col = layout->columnCount();

  layout->addWidget(nw, col / 4, col % 4);
  emit nodeAdded(n->getID(), nw);
}

void MainWindow::createModels() {
  // Create and add Models here
  CycleCommandsModel *cyCoModel = new CycleCommandsModel(this);
  connect(this, SIGNAL(cycleChanged()), cyCoModel, SLOT(updateNext()));

  // NetworkGraphModel *networkGraphModel = new NetworkGraphModel(this);

  CurrentODModel *curODModel = new CurrentODModel(this);
  connect(this, SIGNAL(cycleChanged()), curODModel, SLOT(updateNext()));
  // connect(networkGraphModel, SIGNAL(nodeChanged(uint8_t)), curODModel, SLOT(changeNode(uint8_t)))

  ODDescpriptonModel *oddescrModel = new ODDescpriptonModel(this);
  // connect(networkGraphModel, SIGNAL(nodeChanged(uint8_t)), odDescrModel, SLOT(changeNode(uint8_t)))



  models.append(new PacketHistoryModel(this));
  // models.append(new PythonLogModel(this));
  models.append(new QWTPlotModel(this));
  models.append(curODModel);
  // models.append(networkGraphModel);
  models.append(cyCoModel);
  models.append(oddescrModel);

  QWidget *network = ui->networkGraphContents;
  connect(network, SIGNAL(nodeChanged(uint8_t)), cyCoModel, SLOT(changeNode(uint8_t)));
  connect(network, SIGNAL(nodeChanged(uint8_t)), curODModel, SLOT(changeNode(uint8_t)));
  // connect(network, SIGNAL(nodeChanged(uint8_t)), odDescrModel, SLOT(changeNode(uint8_t)));
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

bool MainWindow::changeCycle(uint32_t cycle) {
  if (machineState == GUIState::STOPPED) {
    if (curCycle != cycle) {
      curCycle = cycle;
      emit cycleChanged();
    }
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
  PluginsWindow *win = new PluginsWindow(this);
  win->show();

  connect(win->getEditor(),
          SIGNAL(pluginsSaved(QMap<QString, QString>)),
          ui->pluginSelectorWidget,
          SLOT(addPlugins(QMap<QString, QString>)));
}

void MainWindow::openInterfacePicker() {
  InterfacePicker *picker = new InterfacePicker(this, getCaptureInstance());
  picker->show();
}

void MainWindow::openSettings() { settingsWin->show(); }

bool MainWindow::event(QEvent *event) {
  // configure stuff
  if (event->type() == QEvent::Polish) {
    qDebug() << "Polish in main";
    if (captureInstance->getState() != CaptureInstance::RUNNING)
      changeState(GUIState::UNINIT);
    // create Models
    createModels();
  }
  return QMainWindow::event(event);
}

uint32_t MainWindow::getCycleNum() { return curCycle; }

CaptureInstance *MainWindow::getCaptureInstance() { return captureInstance.get(); }


void MainWindow::save() {
  // TODO
}
void MainWindow::saveAs() {
  // TODO
}
void MainWindow::open() {
  QString filenames = "Wireshark Files (";
  for (GSList *types = wtap_get_all_file_extensions_list(); types; types = types->next) {
    filenames.append("*.");
    filenames.append(static_cast<char *>(types->data));
    filenames.append(" ");
  }

  filenames.append(")");

  QString curFile = QFileDialog::getOpenFileName(this, tr("Open Captured File"), "", filenames);
  if (curFile == QString::null)
    return;

  file = curFile.toStdString();
  changeState(GUIState::PLAYING);
}

void MainWindow::startRecording() {
  qDebug() << "start Recording";
  changeState(GUIState::RECORDING);
}

void MainWindow::stopRecording() {
  qDebug() << "stop Recording";
  changeState(GUIState::STOPPED);
}

void MainWindow::changeState(GUIState nState) {
  std::string test =
        "Change state from " + EPLVizEnum2Str::toStr(machineState) + " to " + EPLVizEnum2Str::toStr(nState);
  qDebug() << QString::fromStdString(test);
  // TODO other states (and maybe implement a real state machine in modelthread)
  // switch with old state
  switch (machineState) {
    case GUIState::UNINIT: break;
    case GUIState::RECORDING: break;
    case GUIState::PAUSED: break;
    case GUIState::STOPPED: break;
    case GUIState::PLAYING: break;
  }
  // switch with new state
  int backendState;
  switch (nState) {
    case GUIState::UNINIT:
      captureInstance = std::make_unique<CaptureInstance>();
      ui->actionStart_Recording->setEnabled(true);
      ui->actionStop_Recording->setEnabled(false);
      ui->actionSave->setEnabled(false);
      ui->actionSave_As->setEnabled(false);
      break;
    case GUIState::PLAYING:
      config();
      backendState = captureInstance->loadPCAP(file);
      if (backendState != 0) {
        qDebug() << QString("Backend error Code ") + QString::number(backendState);
        changeState(GUIState::UNINIT);
        return;
      }
      break;
    case GUIState::RECORDING:
      config();
      backendState = captureInstance->startRecording(interface.toStdString());
      if (backendState != 0) {
        qDebug() << QString("Backend error Code ") + QString::number(backendState);
        changeState(GUIState::UNINIT);
        return;
      }
      break;
    case GUIState::PAUSED: break;
    case GUIState::STOPPED:
      findChild<QAction *>("actionStop_Recording")->setEnabled(false);
      findChild<QAction *>("actionStart_Recording")->setEnabled(true);
      captureInstance->stopRecording();
      findChild<QAction *>("actionSave")->setEnabled(true);
      findChild<QAction *>("actionSaveAs")->setEnabled(true);
      break;
  }
  machineState = nState;
}

void MainWindow::config() {
  emit recordingStarted(getCaptureInstance());
  captureInstance->getPluginManager()->addPlugin(std::make_shared<plugins::TimeSeriesBuilder>());
  captureInstance->registerCycleStorage<plugins::CSTimeSeriesPtr>(
        EPL_DataCollect::constants::EPL_DC_PLUGIN_TIME_SERIES_CSID);
  findChild<QAction *>("actionStart_Recording")->setEnabled(false);
  findChild<QAction *>("actionStop_Recording")->setEnabled(true);
  BaseModel::initAll(); // TODO do we need to do this here
}

void MainWindow::closeEvent(QCloseEvent *event) {
  profileManager->getDefaultProfile()->writeWindowSettings(this);
  emit close();
  QWidget::closeEvent(event);
}

bool MainWindow::curODWidgetUpdateData(QTreeWidgetItem *item, QString newData) {
  if (item->text(1).compare(newData))
    item->setText(1, newData);
  return true;
}

// Partly disabled because of performance
void MainWindow::externalUpdateCurOD(EPL_DataCollect::Cycle *cycle, int node) {
  QTreeWidget *tree = ui->curNodeODWidget;
  (void)tree;
  Node *n = cycle->getNode(node);
  if (n == nullptr) {
    qDebug() << "Node does not exist, finished update";
    return;
  }
  OD *                  od      = n->getOD();
  auto                  entries = od->getWrittenValues();
  std::vector<uint16_t> entriesVect(entries.begin(), entries.end());
  sort(entriesVect.begin(), entriesVect.end());

  for (uint32_t i = 0; i < entriesVect.size(); i++) {
    uint16_t         odIndex = entriesVect[i];
    ODEntry *        entry   = od->getEntry(odIndex);
    QTreeWidgetItem *topItem = tree->topLevelItem(i);
    if (topItem == nullptr) {
      // Does not exist, create it and subindices
      topItem = new QTreeWidgetItem();
      topItem->setText(0, QString("0x") + QString::number(odIndex, 16));
      topItem->setText(1, QString(""));
      curODWidgetUpdateData(topItem, QString::fromStdString(entry->toString(0)));
      // Create Subindices if needed
      /*
      if (entry->getArraySize() >= 0) {
        for (uint16_t subI = 0; subI < entry->getArraySize(); i++) {
          QTreeWidgetItem *subItem = new QTreeWidgetItem();
          subItem->setText(0, QString("0x") + QString::number(subI, 16));
          curODWidgetUpdateData(subItem, QString::fromStdString(entry->toString(subI)));
          topItem->addChild(subItem);
        }
      }*/
      tree->addTopLevelItem(topItem);
    } else {
      // check for changes
      int children = topItem->childCount();
      if (children == 0) {
        // no children, check top value
        curODWidgetUpdateData(topItem, QString::fromStdString(entry->toString(0)));
      } else {
        // check children
        /*
        for (uint16_t subI = 0; i < UINT8_MAX; i++) {
          curODWidgetUpdateData(topItem->child(subI), QString::fromStdString(entry->toString(subI)));
        }
        */
      }
    }
  }
  qDebug() << "Finished updating currentodmodel";
}

void MainWindow::odDescrWidgetUpdateData(QTreeWidgetItem *item, QVector<QString> newData) {
  if (item->text(1).compare(newData[0]))
    item->setText(1, newData[0]);
  if (item->text(2).compare(newData[1]))
    item->setText(1, newData[1]);
  if (item->text(3).compare(newData[2]))
    item->setText(1, newData[2]);
}

void MainWindow::externalUpdateODDescr(EPL_DataCollect::Cycle *cycle, int node) {
  QTreeWidget *tree = ui->curNodeODWidget;
  (void)tree;
  Node *n = cycle->getNode(node);
  if (n == nullptr) {
    qDebug() << "Node does not exist, finished update";
    return;
  }
  OD *                  od      = n->getOD();
  auto                  entries = od->getWrittenValues();
  std::vector<uint16_t> entriesVect(entries.begin(), entries.end());
  sort(entriesVect.begin(), entriesVect.end());

  for (uint32_t i = 0; i < entriesVect.size(); i++) {
    // uint16_t         odIndex = entriesVect[i];
    // ODEntry *        entry   = od->getEntry(odIndex);
    ODDescription *  descr   = od->getODDesc();
    QTreeWidgetItem *topItem = tree->topLevelItem(i);
    if (topItem == nullptr) {
      // Does not exist, create it and subindices
      topItem = new QTreeWidgetItem();
      topItem->setText(0, QString("0x") + QString::number(descr->getEntry(i)->index, 16));
      topItem->setText(1, QString::fromStdString(EPLEnum2Str::toStr(descr->getEntry(i)->dataType)));
      topItem->setText(2, QString::fromStdString(descr->getEntry(i)->name));
      topItem->setText(3, QString::fromStdString(descr->getEntry(i)->defaultValue->toString()));
      tree->addTopLevelItem(topItem);
    } else {
      // check for changes
      QVector<QString> d;
      d[0] = QString::fromStdString(EPLEnum2Str::toStr(descr->getEntry(i)->dataType));
      d[1] = QString::fromStdString(descr->getEntry(i)->name);
      d[2] = QString::fromStdString(descr->getEntry(i)->defaultValue->toString());
      topItem->setText(0, QString("0x") + QString::number(descr->getEntry(i)->index, 16));
      odDescrWidgetUpdateData(topItem, d);
    }
  }
  qDebug() << "Finished updating currentodmodel";
}

QWidget *MainWindow::getNetworkGraph() { return ui->networkGraphContents; }

SettingsWindow *MainWindow::getSettingsWin() { return settingsWin; }

void MainWindow::handleResults(const QString &result) { qDebug() << "The result is\"" << result << "\""; }
