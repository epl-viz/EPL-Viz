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

#include "settingswindow.hpp"
#include "settingsprofileitem.hpp"
#include "ui_settingswindow.h"
#include <QInputDialog>

#include "mainwindow.hpp"

using namespace EPL_Viz;

SettingsWindow::SettingsWindow(QWidget *parent, ProfileManager *settings)
    : QDialog(parent), ui(new Ui::SettingsWindow) {
  ui->setupUi(this);
  mainWindow = dynamic_cast<MainWindow *>(parent);
  conf       = settings;

  auto profs = conf->getProfiles();

  for (auto i : profs) {
    currentProfile            = i.toStdString();
    profiles[i.toStdString()] = std::make_shared<SettingsProfileItem>(i, ui->profList);
    SettingsProfileItem *prof = profiles[i.toStdString()].get();
    ui->profList->addItem(prof);
    loadConfig();
    if (prof->cfg.nodes.empty()) {
      prof->cfg.nodes[-1]   = mainWindow->getCaptureInstance()->getDefaultNodeConfig();
      prof->cfg.currentNode = -1;
    }
  }

  startCFG.nodes[-1]   = mainWindow->getCaptureInstance()->getDefaultNodeConfig();
  startCFG.currentNode = -1;
  startCFG.backConf    = mainWindow->getCaptureInstance()->getConfig();

  currentProfile = conf->getRawSettings()->value("currentProfile").toString().toStdString();
  if (profiles[currentProfile].get() == nullptr)
    currentProfile = "Default";

  SettingsProfileItem *prof = profiles["Default"].get();
  if (prof->cfg.backConf.ihConfig.eplFrameName.empty()) {
    prof->cfg = startCFG;
  }

  prof = profiles[currentProfile].get();
  ui->profList->setCurrentItem(prof);
  updateProfiles();
}

SettingsWindow::~SettingsWindow() { delete ui; }

void SettingsWindow::updateProfiles() {
  SettingsProfileItem *prof = profiles[currentProfile].get();
  ui->G_XDDDir->setText(prof->cfg.backConf.xddDir.c_str());
  ui->G_PausePlay->setCheckState(prof->cfg.pauseWhilePlayingFile ? Qt::Checked : Qt::Unchecked);
  ui->SM_interval->setValue(static_cast<int>(prof->cfg.backConf.smConfig.saveInterval));
  ui->PY_pluginDIR->setText(prof->cfg.pythonPluginsDir.c_str());
  ui->IH_EPLFrameName->setText(prof->cfg.backConf.ihConfig.eplFrameName.c_str());
  ui->IH_Prefetch->setValue(prof->cfg.backConf.ihConfig.prefetchSize);
  ui->IH_CleanupI->setValue(static_cast<int>(prof->cfg.backConf.ihConfig.cleanupInterval));
  ui->IH_CheckPrefetch->setValue(static_cast<int>(prof->cfg.backConf.ihConfig.checkPrefetch));
  ui->IH_DeleteCyclesAfter->setValue(static_cast<int>(prof->cfg.backConf.ihConfig.deleteCyclesAfter.count()));
  ui->IH_LoopWait->setValue(static_cast<int>(prof->cfg.backConf.ihConfig.loopWaitTimeout.count()));
  ui->nodesList->clear();
  for (auto i : prof->cfg.nodes) {
    QString name = std::to_string(i.first).c_str();
    if (i.first < 0) {
      name = "Default";
    }
    ui->nodesList->addItem(name);
  }
  std::string              name = prof->cfg.currentNode < 0 ? "Default" : std::to_string(prof->cfg.currentNode);
  QList<QListWidgetItem *> list = ui->nodesList->findItems(name.c_str(), Qt::MatchExactly);
  if (list.empty()) {
    qDebug() << "ERROR: No nodes saved";
  } else {
    ui->nodesList->setCurrentItem(*list.begin());
  }
}

void SettingsWindow::saveIntoProfiles() {
  SettingsProfileItem *prof                     = profiles[currentProfile].get();
  prof->cfg.backConf.xddDir                     = ui->G_XDDDir->text().toStdString();
  prof->cfg.pauseWhilePlayingFile               = ui->G_PausePlay->checkState() == Qt::Checked;
  prof->cfg.backConf.smConfig.saveInterval      = static_cast<uint32_t>(ui->SM_interval->value());
  prof->cfg.pythonPluginsDir                    = ui->PY_pluginDIR->text().toStdString();
  prof->cfg.backConf.ihConfig.eplFrameName      = ui->IH_EPLFrameName->text().toStdString();
  prof->cfg.backConf.ihConfig.prefetchSize      = static_cast<uint8_t>(ui->IH_Prefetch->value());
  prof->cfg.backConf.ihConfig.cleanupInterval   = static_cast<uint8_t>(ui->IH_CleanupI->value());
  prof->cfg.backConf.ihConfig.checkPrefetch     = static_cast<uint8_t>(ui->IH_CheckPrefetch->value());
  prof->cfg.backConf.ihConfig.deleteCyclesAfter = std::chrono::milliseconds(ui->IH_DeleteCyclesAfter->value());
  prof->cfg.backConf.ihConfig.loopWaitTimeout   = std::chrono::milliseconds(ui->IH_LoopWait->value());
  prof->cfg.nodes.clear();

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
}

void SettingsWindow::loadConfig() {
  SettingsProfileItem *prof       = profiles[currentProfile].get();
  Profile *            sp         = conf->getProfile(currentProfile.c_str());
  prof->cfg.backConf.xddDir       = sp->readCustomValue("EPL_DC/xddDir").toString().toStdString();
  prof->cfg.pauseWhilePlayingFile = sp->readCustomValue("pauseWhilePlayingFile").toBool();
  prof->cfg.backConf.smConfig.saveInterval =
        static_cast<uint32_t>(sp->readCustomValue("EPL_DC/SM/saveInterval").toInt());
  prof->cfg.pythonPluginsDir               = sp->readCustomValue("pythonPluginsDir").toString().toStdString();
  prof->cfg.backConf.ihConfig.eplFrameName = sp->readCustomValue("EPL_DC/IH/eplFrameName").toString().toStdString();
  prof->cfg.backConf.ihConfig.prefetchSize =
        static_cast<uint8_t>(sp->readCustomValue("EPL_DC/IH/prefetchSize").toInt());
  prof->cfg.backConf.ihConfig.cleanupInterval =
        static_cast<uint8_t>(sp->readCustomValue("EPL_DC/IH/cleanupInterval").toInt());
  prof->cfg.backConf.ihConfig.checkPrefetch =
        static_cast<uint8_t>(sp->readCustomValue("EPL_DC/IH/checkPrefetch").toInt());
  prof->cfg.backConf.ihConfig.deleteCyclesAfter =
        std::chrono::milliseconds(sp->readCustomValue("EPL_DC/IH/deleteCyclesAfter").toInt());
  prof->cfg.backConf.ihConfig.deleteCyclesAfter =
        std::chrono::milliseconds(sp->readCustomValue("EPL_DC/IH/loopWaitTimeout").toInt());

  int size = sp->beginReadArray("nodes");
  for (int i = 0; i < size; ++i) {
    sp->setArrayIndex(i);
    int         index                                = sp->readCustomValue("id").toInt();
    bool        autoDed                              = sp->readCustomValue("autoDeduceSpecificProfile").toBool();
    std::string base                                 = sp->readCustomValue("baseProfile").toString().toStdString();
    std::string specificProfile                      = sp->readCustomValue("specificProfile").toString().toStdString();
    prof->cfg.nodes[index].autoDeduceSpecificProfile = autoDed;
    prof->cfg.nodes[index].baseProfile               = base;
    prof->cfg.nodes[index].specificProfile           = specificProfile;
  }
  sp->endArray();
}

void SettingsWindow::saveConfig() {
  SettingsProfileItem *prof = profiles[currentProfile].get();
  Profile *            sp   = conf->getProfile(currentProfile.c_str());
  sp->writeCustomValue("EPL_DC/xddDir", prof->cfg.backConf.xddDir.c_str());
  sp->writeCustomValue("pauseWhilePlayingFile", prof->cfg.pauseWhilePlayingFile);
  sp->writeCustomValue("EPL_DC/SM/saveInterval", prof->cfg.backConf.smConfig.saveInterval);
  sp->writeCustomValue("pythonPluginsDir", prof->cfg.pythonPluginsDir.c_str());
  sp->writeCustomValue("EPL_DC/IH/eplFrameName", prof->cfg.backConf.ihConfig.eplFrameName.c_str());
  sp->writeCustomValue("EPL_DC/IH/prefetchSize", prof->cfg.backConf.ihConfig.prefetchSize);
  sp->writeCustomValue("EPL_DC/IH/cleanupInterval", prof->cfg.backConf.ihConfig.cleanupInterval);
  sp->writeCustomValue("EPL_DC/IH/checkPrefetch", prof->cfg.backConf.ihConfig.checkPrefetch);
  sp->writeCustomValue("EPL_DC/IH/deleteCyclesAfter",
                       static_cast<int>(prof->cfg.backConf.ihConfig.deleteCyclesAfter.count()));
  sp->writeCustomValue("EPL_DC/IH/loopWaitTimeout",
                       static_cast<int>(prof->cfg.backConf.ihConfig.deleteCyclesAfter.count()));

  sp->beginWriteArray("nodes");
  auto counter = 0;
  for (auto const &i : prof->cfg.nodes) {
    sp->setArrayIndex(counter);
    sp->writeCustomValue("id", i.first);
    sp->writeCustomValue("autoDeduceSpecificProfile", i.second.autoDeduceSpecificProfile);
    sp->writeCustomValue("baseProfile", i.second.baseProfile.c_str());
    sp->writeCustomValue("specificProfile", i.second.specificProfile.c_str());
    qDebug() << "Writing node " << i.first;
    ++counter;
  }
  sp->endArray();

  conf->getRawSettings()->setValue("currentProfile", currentProfile.c_str());
}

void SettingsWindow::apply() {
  SettingsProfileItem *prof = profiles[currentProfile].get();
  saveIntoProfiles();
  saveConfig();
  mainWindow->getCaptureInstance()->setConfig(prof->cfg.backConf);
}

void SettingsWindow::reset() {
  SettingsProfileItem *prof = profiles[currentProfile].get();
  prof->cfg                 = startCFG;
  updateProfiles();
  saveConfig();
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

  saveIntoProfiles();
  saveConfig();
  currentProfile            = newProf;
  profiles[newProf]         = std::make_shared<SettingsProfileItem>(newProf.c_str(), ui->profList);
  SettingsProfileItem *prof = profiles[newProf].get();
  ui->profList->addItem(prof);
  prof->cfg.backConf = mainWindow->getCaptureInstance()->getConfig();
  prof->cfg.nodes.clear();
  prof->cfg.nodes[-1]   = mainWindow->getCaptureInstance()->getDefaultNodeConfig();
  prof->cfg.currentNode = -1;
  saveConfig();
  updateProfiles();
}

void SettingsWindow::deleteProfile() {
  SettingsProfileItem *prof = reinterpret_cast<SettingsProfileItem *>(ui->nodesList->currentItem());

  if (prof->name == "Default")
    return;

  conf->deleteProfile(prof->name);
  profiles[prof->name.toStdString()] = nullptr;
  ui->nodesList->removeItemWidget(prof);
  currentProfile = "Default";
  updateProfiles();
  saveConfig();
}

void SettingsWindow::newNode() {}

void SettingsWindow::deleteNode() {}

void SettingsWindow::profChange(QListWidgetItem *curr, QListWidgetItem *) {
  (void)curr;
  saveIntoProfiles();
  saveConfig();

  SettingsProfileItem *it = dynamic_cast<SettingsProfileItem *>(curr);
  currentProfile          = it->name.toStdString();
  updateProfiles();
}

void SettingsWindow::nodeChange(QListWidgetItem *curr, QListWidgetItem *) {
  (void)curr;
  saveIntoProfiles();
  saveConfig();
}

SettingsProfileItem::Config SettingsWindow::getConfig() { return profiles["Default"].get()->cfg; }
