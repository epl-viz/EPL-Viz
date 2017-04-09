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

#include "SettingsWindow.hpp"
#include "EPLVizDefines.hpp"
#include "SettingsProfileItem.hpp"
#include "SettingsWriter.hpp"
#include "ui_settingswindow.h"
#include <QColorDialog>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>

#include "MainWindow.hpp"

using namespace EPL_Viz;
using namespace EPL_Viz::constants;
using namespace std::chrono;

const QRegExp colSetReg("^COL_S_\\w+$");
const QRegExp colClearReg("^COL_C_\\w+$");

SettingsWindow::SettingsWindow(QWidget *parent, ProfileManager *settings)
    : QDialog(parent), ui(new Ui::SettingsWindow) {
  ui->setupUi(this);
  mainWindow = dynamic_cast<MainWindow *>(parent);
  plotModel  = new SettingsPlotModel(this, ui->plotsView);
  conf       = settings;

  auto profs = conf->getProfiles();

  QList<QToolButton *> cSet   = ui->colors_tabs->findChildren<QToolButton *>(colSetReg);
  QList<QToolButton *> cClear = ui->colors_tabs->findChildren<QToolButton *>(colClearReg);

  for (auto *i : cSet) {
    connect(i, SIGNAL(clicked()), this, SLOT(setColor()));
  }

  for (auto *i : cClear) {
    connect(i, SIGNAL(clicked()), this, SLOT(clearColor()));
  }

  disableList = {ui->c1,
                 ui->widgetBackend_General,
                 ui->widgetBackend_IH,
                 ui->widgetBackend_SM,
                 ui->widgetGUI_General,
                 ui->widgetNode1,
                 ui->widgetNode2,
                 ui->widgetPython,
                 ui->plotClear,
                 ui->plotDelete,
                 ui->plotEdit,
                 ui->plotsView};

  for (auto i : profs) {
    currentProfile            = i.toStdString();
    profiles[currentProfile]  = std::make_shared<SettingsProfileItem>(i, ui->profList);
    SettingsProfileItem *prof = profiles[currentProfile].get();
    ui->profList->addItem(prof);
    loadConfig();
    if (prof->cfg.nodes.empty()) {
      prof->cfg.nodes[-1] = mainWindow->getCaptureInstance()->getDefaultNodeConfig();
    }
    prof->cfg.currentNode = -1;
  }

  startCFG.nodes[-1]   = mainWindow->getCaptureInstance()->getDefaultNodeConfig();
  startCFG.currentNode = -1;
  startCFG.backConf    = mainWindow->getCaptureInstance()->getConfig();

  char *appImageDir = getenv("APPDIR");
  if (!appImageDir) {
    startCFG.backConf.xddDir = std::string(EPL_VIZ_INSTALL_PREFIX) + "/share/eplViz/xdd";
  } else {
    startCFG.backConf.xddDir = std::string(appImageDir) + "/usr/share/eplViz/xdd";
  }

  currentProfile            = "Default";
  SettingsProfileItem *prof = profiles[currentProfile].get();
  if (prof->cfg.backConf.ihConfig.eplFrameName.empty()) {
    prof->cfg = startCFG;
    saveConfig();
  }

  currentProfile = conf->getRawSettings()->value("currentProfile").toString().toStdString();
  if (profiles[currentProfile].get() == nullptr)
    currentProfile = "Default";

  prof = profiles[currentProfile].get();
  ui->profList->setCurrentItem(prof);
  updateView();
}



SettingsWindow::~SettingsWindow() {
  ui->nodesList->disconnect();
  ui->profList->disconnect();
  delete ui;
}

void SettingsWindow::updateView(bool updateNodes) {
  SettingsProfileItem *prof = profiles[currentProfile].get();
  ui->G_XDDDir->setText(prof->cfg.backConf.xddDir.c_str());
  ui->G_PausePlay->setCheckState(prof->cfg.pauseWhilePlayingFile ? Qt::Checked : Qt::Unchecked);
  ui->TL_ChancheCycleImm->setCheckState(prof->cfg.immidiateCycleChange ? Qt::Checked : Qt::Unchecked);
  ui->TL_Invert->setCheckState(prof->cfg.invertTimeLineZoom ? Qt::Checked : Qt::Unchecked);
  ui->G_SleepTime->setValue(static_cast<int>(prof->cfg.guiThreadWaitTime.count()));
  ui->SM_interval->setValue(static_cast<int>(prof->cfg.backConf.smConfig.saveInterval));
  ui->PY_pluginDIR->setText(prof->cfg.pythonPluginsDir.c_str());
  ui->IH_EPLFrameName->setText(prof->cfg.backConf.ihConfig.eplFrameName.c_str());
  ui->IH_Prefetch->setValue(prof->cfg.backConf.ihConfig.prefetchSize);
  ui->IH_CleanupI->setValue(static_cast<int>(prof->cfg.backConf.ihConfig.cleanupInterval));
  ui->IH_CheckPrefetch->setValue(static_cast<int>(prof->cfg.backConf.ihConfig.checkPrefetch));
  ui->IH_DeleteCyclesAfter->setValue(static_cast<int>(prof->cfg.backConf.ihConfig.deleteCyclesAfter.count()));
  ui->IH_LoopWait->setValue(static_cast<int>(prof->cfg.backConf.ihConfig.loopWaitTimeout.count()));
  ui->IH_PreSoCCycles->setCheckState(prof->cfg.backConf.ihConfig.enablePreSOCCycles ? Qt::Checked : Qt::Unchecked);

  std::vector<std::pair<QFrame *, QColor *>> colorSelctor = {
        {ui->COL_ODHighlight, &prof->cfg.odHighlight},
        {ui->COL_P_ASnd, &prof->cfg.pASnd},
        {ui->COL_P_Invalid, &prof->cfg.pInvalid},
        {ui->COL_P_PReq, &prof->cfg.pPReq},
        {ui->COL_P_PResp, &prof->cfg.pPRes},
        {ui->COL_P_SoA, &prof->cfg.pSoA},
        {ui->COL_P_SoC, &prof->cfg.pSoC},
        {ui->COL_P_ANMI, &prof->cfg.pANMI},
        {ui->COL_P_AINV, &prof->cfg.pAINV},

        {ui->COL_ProtoError, &prof->cfg.evProtoError},
        {ui->COL_Error, &prof->cfg.evError},
        {ui->COL_Warning, &prof->cfg.evWarning},
        {ui->COL_Info, &prof->cfg.evInfo},
        {ui->COL_Debug, &prof->cfg.evDebug},
        {ui->COL_PText, &prof->cfg.evPText},

        {ui->COL_nHighlighted, &prof->cfg.NMT_Highlighted},
        {ui->COL_NMTOFF, &prof->cfg.NMT_OFF},
        {ui->COL_INITIALISING, &prof->cfg.NMT_INITIALISING},
        {ui->COL_RESET_APPLICATION, &prof->cfg.NMT_RESET_APPLICATION},
        {ui->COL_RESET_COMMUNICATION, &prof->cfg.NMT_RESET_COMMUNICATION},
        {ui->COL_RESET_CONFIGURATION, &prof->cfg.NMT_RESET_CONFIGURATION},
        {ui->COL_NOT_ACTIVE, &prof->cfg.NMT_NOT_ACTIVE},
        {ui->COL_PRE_OPERATIONAL_1, &prof->cfg.NMT_PRE_OPERATIONAL_1},
        {ui->COL_PRE_OPERATIONAL_2, &prof->cfg.NMT_PRE_OPERATIONAL_2},
        {ui->COL_READY_TO_OPERATE, &prof->cfg.NMT_READY_TO_OPERATE},
        {ui->COL_OPERATIONAL, &prof->cfg.NMT_OPERATIONAL},
        {ui->COL_STOPPED, &prof->cfg.NMT_STOPPED},
        {ui->COL_BASIC_ETHERNET, &prof->cfg.NMT_BASIC_ETHERNET},
  };

  for (auto i : colorSelctor) {
    QPalette pal = palette();

    if (i.second->isValid()) {
      pal.setColor(QPalette::Window, *i.second);
      i.first->setEnabled(true);
    } else {
      i.first->setEnabled(false);
    }
    i.first->setPalette(pal);
    i.first->update();
  }

  if (!updateNodes)
    return;

  std::string              name = prof->cfg.currentNode < 0 ? "Default" : std::to_string(prof->cfg.currentNode);
  QList<QListWidgetItem *> list = ui->nodesList->findItems(name.c_str(), Qt::MatchExactly);
  if (list.empty()) {
    qDebug() << "ERROR: No nodes saved";
  } else {
    ui->N_autoDetect->setCheckState(prof->cfg.nodes[prof->cfg.currentNode].autoDeduceSpecificProfile ? Qt::Checked
                                                                                                     : Qt::Unchecked);
    ui->N_Base->setText(prof->cfg.nodes[prof->cfg.currentNode].baseProfile.c_str());
    ui->N_Special->setText(prof->cfg.nodes[prof->cfg.currentNode].specificProfile.c_str());
    ui->nodesList->setCurrentItem(*list.begin());
  }

  plotModel->update();
}

void SettingsWindow::saveIntoProfiles() {
  SettingsProfileItem *prof                      = profiles[currentProfile].get();
  prof->cfg.backConf.xddDir                      = ui->G_XDDDir->text().toStdString();
  prof->cfg.pauseWhilePlayingFile                = ui->G_PausePlay->checkState() == Qt::Checked;
  prof->cfg.immidiateCycleChange                 = ui->TL_ChancheCycleImm->checkState() == Qt::Checked;
  prof->cfg.invertTimeLineZoom                   = ui->TL_Invert->checkState() == Qt::Checked;
  prof->cfg.guiThreadWaitTime                    = milliseconds(ui->G_SleepTime->value());
  prof->cfg.backConf.smConfig.saveInterval       = static_cast<uint32_t>(ui->SM_interval->value());
  prof->cfg.pythonPluginsDir                     = ui->PY_pluginDIR->text().toStdString();
  prof->cfg.backConf.ihConfig.eplFrameName       = ui->IH_EPLFrameName->text().toStdString();
  prof->cfg.backConf.ihConfig.prefetchSize       = static_cast<uint8_t>(ui->IH_Prefetch->value());
  prof->cfg.backConf.ihConfig.cleanupInterval    = static_cast<uint8_t>(ui->IH_CleanupI->value());
  prof->cfg.backConf.ihConfig.checkPrefetch      = static_cast<uint8_t>(ui->IH_CheckPrefetch->value());
  prof->cfg.backConf.ihConfig.deleteCyclesAfter  = milliseconds(ui->IH_DeleteCyclesAfter->value());
  prof->cfg.backConf.ihConfig.loopWaitTimeout    = milliseconds(ui->IH_LoopWait->value());
  prof->cfg.backConf.ihConfig.enablePreSOCCycles = ui->IH_PreSoCCycles->checkState() == Qt::Checked;


  std::vector<std::pair<QFrame *, QColor *>> colorSelctor = {
        {ui->COL_ODHighlight, &prof->cfg.odHighlight},
        {ui->COL_P_ASnd, &prof->cfg.pASnd},
        {ui->COL_P_Invalid, &prof->cfg.pInvalid},
        {ui->COL_P_PReq, &prof->cfg.pPReq},
        {ui->COL_P_PResp, &prof->cfg.pPRes},
        {ui->COL_P_SoA, &prof->cfg.pSoA},
        {ui->COL_P_SoC, &prof->cfg.pSoC},
        {ui->COL_P_ANMI, &prof->cfg.pANMI},
        {ui->COL_P_AINV, &prof->cfg.pAINV},

        {ui->COL_ProtoError, &prof->cfg.evProtoError},
        {ui->COL_Error, &prof->cfg.evError},
        {ui->COL_Warning, &prof->cfg.evWarning},
        {ui->COL_Info, &prof->cfg.evInfo},
        {ui->COL_Debug, &prof->cfg.evDebug},
        {ui->COL_PText, &prof->cfg.evPText},

        {ui->COL_nHighlighted, &prof->cfg.NMT_Highlighted},
        {ui->COL_NMTOFF, &prof->cfg.NMT_OFF},
        {ui->COL_INITIALISING, &prof->cfg.NMT_INITIALISING},
        {ui->COL_RESET_APPLICATION, &prof->cfg.NMT_RESET_APPLICATION},
        {ui->COL_RESET_COMMUNICATION, &prof->cfg.NMT_RESET_COMMUNICATION},
        {ui->COL_RESET_CONFIGURATION, &prof->cfg.NMT_RESET_CONFIGURATION},
        {ui->COL_NOT_ACTIVE, &prof->cfg.NMT_NOT_ACTIVE},
        {ui->COL_PRE_OPERATIONAL_1, &prof->cfg.NMT_PRE_OPERATIONAL_1},
        {ui->COL_PRE_OPERATIONAL_2, &prof->cfg.NMT_PRE_OPERATIONAL_2},
        {ui->COL_READY_TO_OPERATE, &prof->cfg.NMT_READY_TO_OPERATE},
        {ui->COL_OPERATIONAL, &prof->cfg.NMT_OPERATIONAL},
        {ui->COL_STOPPED, &prof->cfg.NMT_STOPPED},
        {ui->COL_BASIC_ETHERNET, &prof->cfg.NMT_BASIC_ETHERNET},
  };

  for (auto i : colorSelctor) {
    if (i.first->isEnabled()) {
      QPalette pal = i.first->palette();
      *i.second    = pal.color(QPalette::Window);
    } else {
      *i.second = QColor();
    }
  }

  std::string      nID = prof->cfg.currentNode < 0 ? "Default" : std::to_string(prof->cfg.currentNode);
  QListWidgetItem *it  = ui->nodesList->currentItem();
  if (!it) {
    QList<QListWidgetItem *> list = ui->nodesList->findItems(nID.c_str(), Qt::MatchExactly);
    if (list.empty()) {
      qDebug() << "ERROR: No nodes saved";
    } else {
      it = *list.begin();
    }
  }

  if (!it) {
    return;
  }

  std::string name   = it->text().toStdString();
  int         nodeID = -1;
  if (name != "Default") {
    nodeID = static_cast<uint8_t>(std::stoi(name));
  }
  prof->cfg.nodes[nodeID].autoDeduceSpecificProfile = ui->N_autoDetect->checkState() == Qt::Checked;
  prof->cfg.nodes[nodeID].baseProfile               = ui->N_Base->text().toStdString();
  prof->cfg.nodes[nodeID].specificProfile           = ui->N_Special->text().toStdString();

  emit settingsUpdated();
}

void SettingsWindow::loadConfig() {
  SettingsProfileItem *prof = profiles[currentProfile].get();
  Profile *            sp   = conf->getProfile(currentProfile.c_str());
  SettingsWriter       writer(sp);
  prof->readCfg(&writer);
}

void SettingsWindow::saveConfig() {
  SettingsProfileItem *prof = profiles[currentProfile].get();
  Profile *            sp   = conf->getProfile(currentProfile.c_str());
  SettingsWriter       writer(sp);
  prof->writeCfg(&writer);

  conf->getRawSettings()->setValue("currentProfile", currentProfile.c_str());
  conf->getRawSettings()->sync();
}

void SettingsWindow::apply() {
  SettingsProfileItem *prof = profiles[currentProfile].get();
  saveIntoProfiles();
  saveConfig();
  mainWindow->getCaptureInstance()->setConfig(prof->cfg.backConf);
  mainWindow->getCaptureInstance()->setDefaultNodeConfig(prof->cfg.nodes[-1]);
  for (auto i : prof->cfg.nodes) {
    if (i.first < 0)
      continue;

    mainWindow->getCaptureInstance()->setNodeConfig(static_cast<uint8_t>(i.first), i.second);
  }
  updateView();
}

void SettingsWindow::applyOn(EPL_DataCollect::CaptureInstance *ci) {
  SettingsProfileItem *prof = profiles[currentProfile].get();
  ci->setConfig(prof->cfg.backConf);
  for (auto i : prof->cfg.nodes) {
    if (i.first < 0)
      continue;

    ci->setNodeConfig(static_cast<uint8_t>(i.first), i.second);
  }
}

void SettingsWindow::reset() {
  SettingsProfileItem *prof = profiles[currentProfile].get();
  prof->cfg                 = startCFG;
  saveConfig();
  updateView();
}

void SettingsWindow::newProfile() {
  bool        ok;
  std::string newProf;
  QString     title = "Pleas enter the new Profile";
  while (true) {
    newProf = QInputDialog::getText(this, title, "Name: ", QLineEdit::Normal, "", &ok).toStdString();

    if (!ok)
      return;

    if (newProf.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIKLMNOPQRSTUVWXYZ0123456789_") ==
              std::string::npos &&
        !newProf.empty())
      break;

    title = "No special characters are allowed";
  }

  currentProfile            = newProf;
  profiles[newProf]         = std::make_shared<SettingsProfileItem>(newProf.c_str(), ui->profList);
  SettingsProfileItem *prof = profiles[newProf].get();
  ui->profList->addItem(prof);
  if (profiles.find("Default") == profiles.end()) {
    prof->cfg.backConf = mainWindow->getCaptureInstance()->getConfig();
    prof->cfg.nodes.clear();
    prof->cfg.nodes[-1]   = mainWindow->getCaptureInstance()->getDefaultNodeConfig();
    prof->cfg.currentNode = -1;
  } else {
    prof->cfg = profiles["Default"].get()->cfg;
  }
  saveConfig();
  updateView();
}

void SettingsWindow::deleteProfile() {
  SettingsProfileItem *prof = reinterpret_cast<SettingsProfileItem *>(ui->profList->currentItem());

  if (prof->name == "Default")
    return;

  conf->deleteProfile(prof->name);
  profiles[prof->name.toStdString()] = nullptr;
  ui->nodesList->removeItemWidget(prof);
  currentProfile = "Default";
  saveConfig();
  updateView();
}

void SettingsWindow::newNode() {
  bool ok;
  int  nID = QInputDialog::getInt(this, "New Node to add", "Node ID: ", 240, 0, 254, 1, &ok);

  if (!ok)
    return;

  SettingsProfileItem *prof = profiles[currentProfile].get();
  prof->cfg.nodes[nID]      = prof->cfg.nodes[-1];
  prof->cfg.currentNode     = nID;
  ui->nodesList->addItem(new QListWidgetItem(std::to_string(nID).c_str(), ui->nodesList));

  updateView();
}

void SettingsWindow::deleteNode() {
  SettingsProfileItem *prof = profiles[currentProfile].get();
  QListWidgetItem *    nd   = ui->nodesList->currentItem();
  if (!nd)
    return;

  if (nd->text() == "Default")
    return;

  int id = std::stoi(nd->text().toStdString());
  prof->cfg.nodes.erase(id);
  ui->nodesList->takeItem(ui->nodesList->row(nd));
  prof->cfg.currentNode = -1;
  updateView();
}

void SettingsWindow::profChange(QListWidgetItem *curr, QListWidgetItem *) {
  if (!curr) {
    ui->profDelet->setEnabled(false);
    return;
  }

  ui->profDelet->setEnabled(true);

  SettingsProfileItem *it = dynamic_cast<SettingsProfileItem *>(curr);
  currentProfile          = it->name.toStdString();

  loadConfig();

  ui->nodesList->clear();
  SettingsProfileItem *prof = profiles[currentProfile].get();
  for (auto i : prof->cfg.nodes) {
    QString name = std::to_string(i.first).c_str();
    if (i.first < 0) {
      name = "Default";
    }
    ui->nodesList->addItem(new QListWidgetItem(name, ui->nodesList));
  }

  updateView();
}

void SettingsWindow::nodeChange(QListWidgetItem *curr, QListWidgetItem *) {
  if (!curr) {
    ui->nodeRM->setEnabled(false);
    return;
  }

  ui->nodeRM->setEnabled(true);

  SettingsProfileItem *prof = profiles[currentProfile].get();
  int                  id   = -1;
  if (curr->text() != "Default")
    id = std::stoi(curr->text().toStdString());

  prof->cfg.nodes[prof->cfg.currentNode].autoDeduceSpecificProfile = ui->N_autoDetect->checkState() == Qt::Checked;
  prof->cfg.nodes[prof->cfg.currentNode].baseProfile               = ui->N_Base->text().toStdString();
  prof->cfg.nodes[prof->cfg.currentNode].specificProfile           = ui->N_Special->text().toStdString();

  prof->cfg.currentNode = id;
  updateView(true);
  // saveConfig();
}

void SettingsWindow::selectXDDDir() {
  QFileDialog dialog(this, "Select the XDD directory", ui->G_XDDDir->text());
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setFileMode(QFileDialog::Directory);
  dialog.setOption(QFileDialog::ReadOnly, true);
  dialog.setViewMode(QFileDialog::Detail);

  if (dialog.exec()) {
    QStringList files;
    files = dialog.selectedFiles();
    ui->G_XDDDir->setText(files.front());
  }
}

void SettingsWindow::selectPythonDir() {
  QFileDialog dialog(this, "Select the Python plugin directory", ui->PY_pluginDIR->text());
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  dialog.setFileMode(QFileDialog::Directory);
  dialog.setOption(QFileDialog::ReadOnly, true);
  dialog.setViewMode(QFileDialog::Detail);

  if (dialog.exec()) {
    QStringList files;
    files = dialog.selectedFiles();
    ui->PY_pluginDIR->setText(files.front());
  }
}

const QRegExp colorRegex("^COL_\\w+$");

void SettingsWindow::setColor() {
  QToolButton *btn = dynamic_cast<QToolButton *>(sender());

  if (!btn)
    return;

  QPalette pal = palette();

  QColor newColor = QColorDialog::getColor(pal.color(QPalette::Window), this, "Select color");
  if (!newColor.isValid())
    return;

  pal.setColor(QPalette::Window, newColor);

  QList<QFrame *> foundChildren = btn->parent()->findChildren<QFrame *>(colorRegex, Qt::FindDirectChildrenOnly);
  if (foundChildren.empty()) {
    qDebug() << "Internal widget structure error";
    return;
  }

  QFrame *frame = foundChildren.first();
  frame->setPalette(pal);
  frame->setEnabled(true);
  frame->update();
  frame->repaint();
}

void SettingsWindow::clearColor() {
  QToolButton *btn = dynamic_cast<QToolButton *>(sender());

  if (!btn)
    return;

  QPalette pal = palette();

  QList<QFrame *> foundChildren = btn->parent()->findChildren<QFrame *>(colorRegex, Qt::FindDirectChildrenOnly);
  if (foundChildren.empty()) {
    qDebug() << "Internal widget structure error";
    return;
  }

  QFrame *frame = foundChildren.first();

  frame->setPalette(pal);
  frame->setEnabled(false);
  frame->update();
  frame->repaint();
}

void SettingsWindow::plotNew() {
  SettingsProfileItem *prof   = profiles[currentProfile].get();
  auto                 newPlt = PlotCreator::getNewPlot();

  if (!newPlt.isOK)
    return;

  prof->cfg.plots.emplace_back(newPlt);
  updateView();
}

void SettingsWindow::plotEdit() {
  SettingsProfileItem *prof     = profiles[currentProfile].get();
  QModelIndex          currItem = ui->plotsView->currentIndex();

  if (!currItem.isValid())
    return;

  size_t row = plotModel->getItem(currItem)->row();
  if (row >= prof->cfg.plots.size()) {
    qDebug() << "ERROR: Invalid Size";
    return;
  }

  auto newPlt = PlotCreator::getNewPlot(&prof->cfg.plots[row]);
  if (!newPlt.isOK)
    return;

  prof->cfg.plots[row] = newPlt;
  updateView();
}

void SettingsWindow::plotDelete() {
  SettingsProfileItem *prof     = profiles[currentProfile].get();
  QModelIndex          currItem = ui->plotsView->currentIndex();

  if (!currItem.isValid())
    return;

  size_t row = plotModel->getItem(currItem)->row();
  if (row >= prof->cfg.plots.size()) {
    qDebug() << "ERROR: Invalid Size";
    return;
  }

  prof->cfg.plots.erase(prof->cfg.plots.begin() + static_cast<long>(row));
  updateView();
}

void SettingsWindow::plotClear() {
  auto btn = QMessageBox::question(this, "About to delete ALL plots", "Are you sure you want to delete ALL plots?");
  if (btn != QMessageBox::Yes)
    return;

  SettingsProfileItem *prof = profiles[currentProfile].get();
  prof->cfg.plots.clear();
  updateView();
}

void SettingsWindow::createPlot(uint8_t nodeID, uint16_t index, uint8_t subIndex, std::string cs, QColor color) {
  show();
  ui->tabWidget->setCurrentIndex(2);

  SettingsProfileItem *prof = profiles[currentProfile].get();

  PlotCreator::PlotCreatorData pData;
  pData.index         = index;
  pData.subIndex      = subIndex;
  pData.node          = nodeID;
  pData.csName        = cs;
  pData.defaultODPlot = cs.empty();
  pData.color         = color;
  pData.addToPlot     = true;
  pData.addToTimeLine = true;

  auto newPlt = PlotCreator::getNewPlot(&pData);
  if (!newPlt.isOK)
    return;

  prof->cfg.plots.emplace_back(newPlt);
  updateView();
}

const QString exportFileFilter = "EPL-Viz Profiles (*.eplProf);; All Files (*)";

void SettingsWindow::exportProf() {
  SettingsProfileItem *prof = profiles[currentProfile].get();
  QString              file = QFileDialog::getSaveFileName(this, "Export profile file", "", exportFileFilter);

  if (file == "")
    return;

  prof->exportProf(file);
}

void SettingsWindow::importProf() {
  SettingsProfileItem *prof = profiles[currentProfile].get();
  QString              file = QFileDialog::getOpenFileName(this, "Import profile file", "", exportFileFilter);

  if (file == "")
    return;

  prof->importProf(file);
  updateView();
}


int SettingsWindow::execPlotsTab() {
  ui->tabWidget->setCurrentIndex(2);
  return exec();
}

void SettingsWindow::enterRecordingState() {
  for (auto *i : disableList)
    i->setEnabled(false);
}

void SettingsWindow::leaveRecordingState() {
  for (auto *i : disableList)
    i->setEnabled(true);
}

SettingsProfileItem::Config SettingsWindow::getConfig() { return profiles[currentProfile].get()->cfg; }
Profile *                   SettingsWindow::getCurrentProfile() { return conf->getProfile(currentProfile.c_str()); }
