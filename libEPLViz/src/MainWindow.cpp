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
 * \file MainWindow.cpp
 */
#include "MainWindow.hpp"
#include "CSTimeSeriesPtr.hpp"
#include "CycleCommandsModel.hpp"
#include "EPLVizEnum2Str.hpp"
#include "InterfacePicker.hpp"
#include "NetworkGraphModel.hpp"
#include "ODDescriptionModel.hpp"
#include "PluginsWindow.hpp"
#include "ProtocolValidator.hpp"
#include "SettingsWindow.hpp"
#include "SettingsWindow.hpp"
#include "TimeLineModel.hpp"
#include "TimeSeriesBuilder.hpp"
#include <QWidgetAction>
#include <memory>
#include <vector>
#include <wiretap/wtap.h>

// Namespace linker error fix
void initResources();
void cleanupResources();

void initResources() { Q_INIT_RESOURCE(resources); }
void cleanupResources() { Q_CLEANUP_RESOURCE(resources); }

using namespace EPL_Viz;
#include "ui_mainwindow.h"

using namespace EPL_DataCollect;


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  initResources();

  machineState = GUIState::UNINIT;

  profileManager = new ProfileManager();

  ui->setupUi(this);
  tabifyDockWidget(ui->dockCurrent, ui->dockOD);
  tabifyDockWidget(ui->dockPlugins, ui->dockEvents);
  ui->actionOD_Filter_2->setMenu(ui->menuOD_Filter);

  std::vector<QAction *> btns;
  btns.emplace_back(ui->actionOD_Filter_2);
  fixQToolButtons(btns, ui->toolBar);

  CS = new CycleSetterAction(ui->toolBar, this);
  ui->toolBar->addAction(CS);

  connect(this,
          SIGNAL(recordingStarted(EPL_DataCollect::CaptureInstance *)),
          ui->pluginSelectorWidget,
          SLOT(loadPlugins(
                EPL_DataCollect::CaptureInstance *))); // Notify the PluginSelectorWidget of the start of recording
  connect(this,
          SIGNAL(recordingStarted(EPL_DataCollect::CaptureInstance *)),
          ui->eventLog,
          SLOT(start(EPL_DataCollect::CaptureInstance *))); // Notify the eventLog of the start of recording

  profileManager->getDefaultProfile()->readWindowSettings(this);
  captureInstance = std::make_unique<CaptureInstance>();

  settingsWin = new SettingsWindow(this, profileManager);
  settingsWin->hide();

  ui->pluginSelectorWidget->setMainWindow(this);
}

MainWindow::~MainWindow() {
  cleanupResources();
  destroyModels();
  delete profileManager;
  delete ui;
  delete settingsWin;
}

void MainWindow::createModels() {
  // Create and add Models here
  CycleCommandsModel *cyCoModel          = new CycleCommandsModel(this, ui->cycleCommandsView);
  QWTPlotModel *      qwtPlot            = new QWTPlotModel(this, ui->qwtPlot);
  NetworkGraphModel * networkGraphModel  = new NetworkGraphModel(this, ui->networkGraphContents);
  ODDescriptionModel *oddescrModel       = new ODDescriptionModel(this, ui->odDescriptionWidget);
  CurrentODModel *    curODModel         = new CurrentODModel(this, ui->curNodeODWidget);
  PythonLogModel *    pythonLogModel     = new PythonLogModel(this, ui->pythonLogView);
  PacketHistoryModel *packetHistoryModel = new PacketHistoryModel(this, ui->packetHistoryTextEdit);
  TimeLineModel *     timeLineModel      = new TimeLineModel(this, ui->qwtPlotTimeline);

  // Connect required signals
  connect(this, SIGNAL(cycleChanged()), cyCoModel, SLOT(updateNext()));
  connect(this, SIGNAL(cycleChanged()), curODModel, SLOT(updateNext()));

  connect(ui->cycleCommandsView,
          SIGNAL(clicked(QModelIndex)),
          cyCoModel,
          SLOT(changeSelection(QModelIndex))); // Notify the cycle viewer model that the selection changed
  connect(cyCoModel,
          SIGNAL(packetChanged(uint64_t)),
          packetHistoryModel,
          SLOT(changePacket(uint64_t))); // Notify the packet viewer of changing packets

  connect(packetHistoryModel,
          SIGNAL(textUpdated(QString, QPlainTextEdit *)),
          ui->dockPacketHistory,
          SLOT(updatePacketHistoryLog(QString, QPlainTextEdit *)));
  // TODO need to emit
  connect(ui->scrBarTimeline, SIGNAL(valueChanged(int)), timeLineModel, SLOT(updateViewport(int)));
  connect(timeLineModel, SIGNAL(maxValueChanged(int, int)), ui->scrBarTimeline, SLOT(setRange(int, int)));
  // TODO update timeline value
  // connect(this, SIGNAL(cycleChanged()), ui->scrBarTimeline, SLOT(setValue(int)))
  // Set timeline max value once, since we can't do this in the constructor of the model and want to do it before init
  emit timeLineModel->maxValueChanged(0, static_cast<int>(timeLineModel->maxXValue));

  // Activate and connect rightclick menu for Drawing Plots
  ui->curNodeODWidget->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(ui->curNodeODWidget,
          SIGNAL(customContextMenuRequested(const QPoint &)),
          curODModel,
          SLOT(showContextMenu(const QPoint &)));
  connect(curODModel,
          SIGNAL(drawingPlot(uint8_t, uint16_t, uint16_t)),
          timeLineModel,
          SLOT(createPlot(uint8_t, uint16_t, uint16_t)));
  connect(curODModel,
          SIGNAL(drawingPlot(uint8_t, uint16_t, uint16_t)),
          qwtPlot,
          SLOT(createPlot(uint8_t, uint16_t, uint16_t)));
  connect(ui->actionSetup_Plot, SIGNAL(triggered()), timeLineModel, SLOT(setupPlotting()));


  // Append the nodes to a list for cleanup
  models.append(packetHistoryModel);
  models.append(pythonLogModel);
  models.append(qwtPlot);
  models.append(curODModel);
  models.append(networkGraphModel);
  models.append(cyCoModel);
  models.append(oddescrModel);
  models.append(timeLineModel);

  QWidget *network = ui->networkGraphContents;
  connect(network, SIGNAL(nodeSelected(uint8_t)), curODModel, SLOT(changeNode(uint8_t)));
  connect(network, SIGNAL(nodeSelected(uint8_t)), oddescrModel, SLOT(changeNode(uint8_t)));


  modelThread = new ModelThread(this, &machineState, this);
  connect(modelThread, &ModelThread::resultReady, this, &MainWindow::handleResults);
  connect(modelThread, &ModelThread::finished, modelThread, &QObject::deleteLater);
  connect(modelThread,
          SIGNAL(updateCompleted(ProtectedCycle &)),
          this,
          SLOT(updateWidgets(ProtectedCycle &)),
          Qt::BlockingQueuedConnection);

  modelThread->start();
  connect(this, SIGNAL(close()), modelThread, SLOT(stop()));
}

void MainWindow::destroyModels() {
  while (!models.isEmpty()) {
    delete models.takeFirst();
  }
}

void MainWindow::updateWidgets(ProtectedCycle &cycle) {
  // TODO: Check if this can be done in a cleaner way
  ui->networkGraphContents->updateWidget(cycle);
  ui->eventLog->updateEvents();
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

bool MainWindow::changeCycle(uint32_t cycle) {
  if (machineState != GUIState::STOPPED) {
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
  QString name = InterfacePicker::getInterface(this, captureInstance.get());
  if (!name.isEmpty())
    interface = name;
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
  // Abort when the captureInstance is invalid
  if (!captureInstance)
    return;

  // Check if a save file has been set yet
  if (saveFile == QString()) {
    // Get a file name
    saveAs();
    return;
  }

  // Delete the old save file if it exists
  if (QFile::exists(saveFile)) {
    QFile::remove(saveFile);
  }

  // The file could not be written
  if (!QFile::copy(QString::fromStdString(captureInstance->getCurrentFilePath()), saveFile)) {
    QErrorMessage error(this);

    error.showMessage("Could not save to file '" + saveFile + "'.");
  }
}

void MainWindow::saveAs() {
  // Abort when the captureInstance is invalid
  if (!captureInstance)
    return;

  QString ws = "Wireshark Files (";
  for (GSList *types = wtap_get_all_file_extensions_list(); types; types = types->next) {
    ws.append("*.");
    ws.append(static_cast<char *>(types->data));
    ws.append(" ");
  }

  ws.append(")");

  QString fileName = QFileDialog::getSaveFileName(this, tr("Save Recording"), "", ws);

  // Check if user selected a file
  if (fileName == QString::null)
    return;

  saveFile = fileName;
  save();
}
void MainWindow::open() {
  QString ws = "Wireshark Files (";
  for (GSList *types = wtap_get_all_file_extensions_list(); types; types = types->next) {
    ws.append("*.");
    ws.append(static_cast<char *>(types->data));
    ws.append(" ");
  }

  ws.append(")");

  QString curFile = QFileDialog::getOpenFileName(this, tr("Open Capture File"), "", ws);
  if (curFile == QString::null)
    return;

  file = curFile.toStdString();
  changeState(GUIState::UNINIT);
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
  // TODO other states (and maybe implement a real state machine in ModelThread)
  // switch with old state
  switch (machineState) {
    case GUIState::UNINIT: break;
    case GUIState::RECORDING:
    case GUIState::PLAYING:
    case GUIState::PAUSED: break;
    case GUIState::STOPPED: break;
  }

  std::string interfaceName;
  // switch with new state
  int backendState;
  switch (nState) {
    case GUIState::UNINIT:
      captureInstance = std::make_unique<CaptureInstance>();
      ui->actionStart_Recording->setEnabled(true);
      ui->actionStop_Recording->setEnabled(false);
      ui->pushButton->setEnabled(true);
      ui->actionPlugins->setEnabled(true);
      ui->actionLoad->setEnabled(true);
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
      if (interface.isEmpty())
        interfaceName = nullptr;
      else
        interfaceName = interface.toStdString();
      backendState    = captureInstance->startRecording(interfaceName);
      if (backendState != 0) {
        qDebug() << QString("Backend error Code ") + QString::number(backendState);
        changeState(GUIState::UNINIT);
        return;
      }
      break;
    case GUIState::PAUSED: break;
    case GUIState::STOPPED:
      ui->actionStart_Recording->setEnabled(false); // TODO: Do not allow until a reset has been done
      ui->actionStop_Recording->setEnabled(false);
      captureInstance->stopRecording();
      ui->actionSave->setEnabled(true);
      ui->actionSave_As->setEnabled(true);
      break;
  }
  machineState = nState;
}

void MainWindow::config() {
  curCycle = UINT32_MAX;
  emit recordingStarted(getCaptureInstance());

  settingsWin->applyOn(captureInstance.get());


  auto plgManager = captureInstance->getPluginManager();

  plgManager->addPlugin(std::make_shared<plugins::TimeSeriesBuilder>());
  plgManager->addPlugin(std::make_shared<plugins::ProtocolValidator>());

  captureInstance->registerCycleStorage<plugins::CSTimeSeriesPtr>(
        EPL_DataCollect::constants::EPL_DC_PLUGIN_TIME_SERIES_CSID);
  ui->actionStart_Recording->setEnabled(false);
  ui->actionStop_Recording->setEnabled(true);
  ui->actionLoad->setEnabled(false);
  ui->actionSave->setEnabled(false);
  ui->actionSave_As->setEnabled(false);
  ui->pushButton->setEnabled(false);
  ui->actionPlugins->setEnabled(false);
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

SettingsWindow *   MainWindow::getSettingsWin() { return settingsWin; }
CycleSetterAction *MainWindow::getCycleSetter() { return CS; }

void MainWindow::handleResults(const QString &result) { qDebug() << "The result is\"" << result << "\""; }
