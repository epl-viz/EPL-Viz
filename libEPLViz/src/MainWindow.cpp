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
#include "CSViewFilters.hpp"
#include "CycleCommandsModel.hpp"
#include "DefaultFilter.hpp"
#include "EPLVizDefines.hpp"
#include "EPLVizEnum2Str.hpp"
#include "InterfacePicker.hpp"
#include "NetworkGraphModel.hpp"
#include "ODDescriptionModel.hpp"
#include "PacketListModel.hpp"
#include "PluginsWindow.hpp"
#include "ProtocolValidator.hpp"
#include "SettingsWindow.hpp"
#include "SettingsWindow.hpp"
#include "Statistics.hpp"
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
using namespace EPL_Viz::constants;
#include "ui_mainwindow.h"

using namespace EPL_DataCollect;
using namespace EPL_DataCollect::constants;
using namespace EPL_DataCollect::plugins;


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  initResources();

  machineState = GUIState::UNINIT;

  profileManager = new ProfileManager();

  ui->setupUi(this);
  tabifyDockWidget(ui->dockCurrent, ui->dockOD);

  CS = new CycleSetterAction(ui->toolBar, this);
  ui->toolBar->addAction(CS);

  connect(this,
          SIGNAL(recordingStarted(EPL_DataCollect::CaptureInstance *)),
          ui->pluginSelectorWidget,
          SLOT(loadPlugins(
                EPL_DataCollect::CaptureInstance *))); // Notify the PluginSelectorWidget of the start of recording
  connect(this,
          SIGNAL(recordingStarted(EPL_DataCollect::CaptureInstance *)),
          ui->eventViewer,
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

bool MainWindow::changeTime(double) { return true; }

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
  PacketListModel *   packetListModel    = new PacketListModel(this, ui->packetsView);

  // Connect required signals
  connect(this, SIGNAL(cycleChanged()), cyCoModel, SLOT(updateNext()));
  connect(this, SIGNAL(cycleChanged()), curODModel, SLOT(updateNext()));

  connect(ui->cycleCommandsView,
          SIGNAL(activated(QModelIndex)),
          cyCoModel,
          SLOT(changeSelection(QModelIndex))); // Notify the cycle viewer model that an item was activated
  connect(ui->cycleCommandsView,
          SIGNAL(clicked(QModelIndex)),
          cyCoModel,
          SLOT(changeSelection(QModelIndex))); // Notify the cycle viewer model that an item was clicked
  connect(cyCoModel,
          SIGNAL(packetChanged(uint64_t)),
          packetHistoryModel,
          SLOT(changePacket(uint64_t))); // Notify the packet viewer of changing packets

  connect(ui->scrBarTimeline, SIGNAL(valueChanged(int)), timeLineModel, SLOT(updateViewport(int)));
  connect(timeLineModel, SIGNAL(maxValueChanged(int, int)), ui->scrBarTimeline, SLOT(setRange(int, int)));
  connect(this, SIGNAL(cycleChanged()), timeLineModel, SLOT(replot()));
  connect(timeLineModel,
          SIGNAL(setNodes(uint8_t, uint16_t, uint16_t)),
          qwtPlot,
          SLOT(createPlot(uint8_t, uint16_t, uint16_t)));

  // Set timeline max value once, since we can't do this in the constructor of the model and want to do it before init
  emit timeLineModel->maxValueChanged(0, static_cast<int>(timeLineModel->maxXValue - timeLineModel->getViewportSize()));

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
  models.append(packetListModel);

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

  // Connect reset signal to widgets requiring it
  connect(this, SIGNAL(resetGUI()), ui->networkGraphContents, SLOT(reset()));
  connect(this, SIGNAL(resetGUI()), ui->eventViewer, SLOT(reset()));
  connect(this, SIGNAL(resetGUI()), ui->pluginSelectorWidget, SLOT(reset()));
  connect(this, SIGNAL(resetGUI()), timeLineModel, SLOT(reset()));
  connect(this, SIGNAL(resetGUI()), qwtPlot, SLOT(reset()));


  modelThread->start();
  connect(this, SIGNAL(close()), modelThread, SLOT(stop()));
}

void MainWindow::destroyModels() {
  while (!models.isEmpty()) {
    delete models.takeFirst();
  }
}

void MainWindow::updateWidgets(ProtectedCycle &cycle) {
  ui->networkGraphContents->updateWidget(cycle);
  ui->eventViewer->updateEvents();

  if (machineState == GUIState::RECORDING || machineState == GUIState::PLAYING || machineState == GUIState::PAUSED) {
    if (captureInstance->getState() == CaptureInstance::DONE) {
      changeState(GUIState::STOPPED);
    }
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

bool MainWindow::changeCycle(uint32_t cycle) {
  if (machineState != GUIState::UNINIT) {
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
  PluginsWindow *win = PluginsWindow::create(this);

  // Check if the window was newly created
  if (win) {
    win->show();

    connect(win->getEditor(),
            SIGNAL(pluginsSaved(QMap<QString, QString>)),
            ui->pluginSelectorWidget,
            SLOT(addPlugins(QMap<QString, QString>)));
  }
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


void MainWindow::newSession() { changeState(GUIState::UNINIT); }

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
    QMessageBox::critical(0, "Error", tr("Could not save to file '%1'.").arg(saveFile));
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

void MainWindow::showAbout() {
  QMessageBox msgBox;

  QString version = "Version ";
  version += std::to_string(EPL_VIZ_VERSION_MAJOR).c_str();
  version += ".";
  version += std::to_string(EPL_VIZ_VERSION_MINOR).c_str();
  version += ".";
  version += std::to_string(EPL_VIZ_VERSION_SUBMINOR).c_str();
  version += " +";
  version += std::to_string(EPL_VIZ_GIT_LAST_TAG_DIFF).c_str();
  version += " Commit ";
  version += EPL_VIZ_VERSION_GIT;

  msgBox.setText("Created by EPL-Vizards. Copyright (c) 2017\n\n" + version);
  msgBox.exec();
}

void MainWindow::showLicense() {
  QMessageBox msgBox;
  msgBox.setText("Copyright (c) 2017, EPL-Vizards\n"
                 "All rights reserved.\n"
                 "\n"
                 "Redistribution and use in source and binary forms, with or without\n"
                 "modification, are permitted provided that the following conditions are met:\n"
                 "    * Redistributions of source code must retain the above copyright\n"
                 "      notice, this list of conditions and the following disclaimer.\n"
                 "    * Redistributions in binary form must reproduce the above copyright\n"
                 "      notice, this list of conditions and the following disclaimer in the\n"
                 "      documentation and/or other materials provided with the distribution.\n"
                 "    * Neither the name of the EPL-Vizards nor the\n"
                 "      names of its contributors may be used to endorse or promote products\n"
                 "      derived from this software without specific prior written permission.\n"
                 "\n"
                 "THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS \"AS IS\" AND\n"
                 "ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED\n"
                 "WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE\n"
                 "DISCLAIMED. IN NO EVENT SHALL EPL-Vizards BE LIABLE FOR ANY\n"
                 "DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES\n"
                 "(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;\n"
                 "LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND\n"
                 "ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT\n"
                 "(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS\n"
                 "SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.\n");
  msgBox.exec();
}

void MainWindow::showStats() { Statistics(this).exec(); }

void MainWindow::startRecording() {
  openInterfacePicker();

  if (!interface.isEmpty()) {
    qDebug() << "start Recording";
    changeState(GUIState::RECORDING);
  }
}

void MainWindow::stopRecording() {
  qDebug() << "stop Recording";
  changeState(GUIState::STOPPED);
}

void MainWindow::changeState(GUIState nState) {
  std::string test =
        "Change state from " + EPLVizEnum2Str::toStr(machineState) + " to " + EPLVizEnum2Str::toStr(nState);
  qDebug() << QString::fromStdString(test);

  // switch with new state
  int backendState;
  switch (nState) {
    case GUIState::UNINIT:
      // Update GUI button states
      ui->actionStart_Recording->setEnabled(true);
      ui->actionStop_Recording->setEnabled(false);
      ui->pluginEditorButton->setEnabled(true);
      ui->actionPlugins->setEnabled(true);
      ui->actionNew->setEnabled(true);
      ui->actionLoad->setEnabled(true);
      ui->actionSave->setEnabled(false);
      ui->actionSave_As->setEnabled(false);
      ui->actionStatistics->setEnabled(false);

      if (machineState == GUIState::STOPPED) {
        // Reset all models back to their initial state
        BaseModel::initAll();
        CS->getWidget()->setValue(0);
        emit resetGUI();
      }

      captureInstance = std::make_unique<CaptureInstance>();
      break;
    case GUIState::PLAYING:
      // Update GUI button states
      ui->actionStart_Recording->setEnabled(false);
      ui->actionStop_Recording->setEnabled(true);
      ui->pluginEditorButton->setEnabled(false);
      ui->actionPlugins->setEnabled(false);
      ui->actionNew->setEnabled(false);
      ui->actionLoad->setEnabled(false);
      ui->actionSave->setEnabled(false); // Saving is not available during playback
      ui->actionSave_As->setEnabled(false);
      ui->actionStatistics->setEnabled(false);

      config();

      backendState = captureInstance->loadPCAP(file);

      // Handle Backend errors
      if (backendState != 0) {
        qDebug() << QString("Backend error Code ") + QString::number(backendState);
        changeState(GUIState::UNINIT);
        return;
      }
      break;
    case GUIState::RECORDING:
      // Update GUI button states
      ui->actionStart_Recording->setEnabled(false);
      ui->actionStop_Recording->setEnabled(true);
      ui->pluginEditorButton->setEnabled(false);
      ui->actionPlugins->setEnabled(false);
      ui->actionNew->setEnabled(false);
      ui->actionLoad->setEnabled(false);
      ui->actionSave->setEnabled(true);
      ui->actionSave_As->setEnabled(true);
      ui->actionStatistics->setEnabled(false);

      config();

      backendState = captureInstance->startRecording(interface.toStdString());

      // Handle Backend errors
      if (backendState != 0) {
        qDebug() << QString("Backend error Code ") + QString::number(backendState);
        changeState(GUIState::UNINIT);
        return;
      }
      break;
    case GUIState::PAUSED:
      // Update GUI button states
      ui->actionStart_Recording->setEnabled(true);
      ui->actionStop_Recording->setEnabled(false);
      ui->pluginEditorButton->setEnabled(false);
      ui->actionPlugins->setEnabled(false);
      ui->actionNew->setEnabled(false);
      ui->actionLoad->setEnabled(false);

      if (captureInstance->getState() == CaptureInstance::DONE) {
        ui->actionStatistics->setEnabled(true);
        ui->actionSave->setEnabled(true);
        ui->actionSave_As->setEnabled(true);
      } else {
        ui->actionStatistics->setEnabled(false);
        ui->actionSave->setEnabled(false);
        ui->actionSave_As->setEnabled(false);
      }

      break;
    case GUIState::STOPPED:
      // Update GUI button states
      ui->actionStart_Recording->setEnabled(false);
      ui->actionStop_Recording->setEnabled(false);
      ui->pluginEditorButton->setEnabled(false);
      ui->actionPlugins->setEnabled(false);
      ui->actionNew->setEnabled(true);
      ui->actionLoad->setEnabled(false);
      ui->actionSave->setEnabled(true);
      ui->actionSave_As->setEnabled(true);
      ui->actionStatistics->setEnabled(true);

      ui->pluginSelectorWidget->setEnabled(false);
      break;
  }
  machineState = nState;
}

void MainWindow::config() {
  curCycle = UINT32_MAX;
  // Notify widgets that recording/playback has started
  emit recordingStarted(getCaptureInstance());
  CS->getWidget()->clearFilters();

  // Apply settings
  settingsWin->applyOn(captureInstance.get());


  auto *plgManager = captureInstance->getPluginManager();

  plgManager->addPlugin(std::make_shared<TimeSeriesBuilder>());
  plgManager->addPlugin(std::make_shared<ProtocolValidator>());
  plgManager->addPlugin(std::make_shared<DefaultFilter>());

  // Register timeseries cycle storage
  captureInstance->registerCycleStorage<plugins::CSTimeSeriesPtr>(
        EPL_DataCollect::constants::EPL_DC_PLUGIN_TIME_SERIES_CSID);

  // Initialize all Models
  BaseModel::initAll(); // TODO do we need to do this here
}

void MainWindow::setFilters(std::vector<EPL_DataCollect::CSViewFilters::Filter> f) {
  filters = f;
  CS->getWidget()->setFilters(filters);
}

EPL_DataCollect::CSViewFilters::Filter MainWindow::getFilter() {
  std::string filter = CS->getWidget()->getCurrentFilter();

  for (auto &i : filters) {
    if (i.getName() == filter) {
      return i;
    }
  }

  return CSViewFilters::Filter(FilterType::EXCLUDE, "All");
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
