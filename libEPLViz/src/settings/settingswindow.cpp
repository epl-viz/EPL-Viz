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

#include "mainwindow.hpp"

SettingsWindow::SettingsWindow(QWidget *parent) : QDialog(parent), ui(new Ui::SettingsWindow) {
  ui->setupUi(this);
  mainWindow                = dynamic_cast<MainWindow *>(parent);
  profiles["Default"]       = std::make_shared<SettingsProfileItem>("Default", ui->profList);
  SettingsProfileItem *prof = profiles["Default"].get();
  ui->profList->addItem(prof);
  prof->cfg.backConf    = mainWindow->getCaptureInstance()->getConfig();
  prof->cfg.nodes[-1]   = mainWindow->getCaptureInstance()->getDefaultNodeConfig();
  prof->cfg.currentNode = -1;
  currentProfile        = "Default";

  updateProfiles();
}

SettingsWindow::~SettingsWindow() { delete ui; }

void SettingsWindow::updateProfiles() {
  SettingsProfileItem *prof = profiles[currentProfile].get();
  ui->G_XDDDir->setText(prof->cfg.backConf.xddDir.c_str());
  ui->SM_interval->setValue(prof->cfg.backConf.smConfig.saveInterval);
  ui->PY_pluginDIR->setText(prof->cfg.pythonPluginsDir.c_str());
}

void SettingsWindow::apply() {}

void SettingsWindow::reset() {}

void SettingsWindow::newProfile() {}

void SettingsWindow::deleteProfile() {}

void SettingsWindow::newNode() {}

void SettingsWindow::deleteNode() {}

void SettingsWindow::profChange(QListWidgetItem *curr, QListWidgetItem *pref) {
  (void)curr;
  (void)pref;
}

void SettingsWindow::nodeChange(QListWidgetItem *curr, QListWidgetItem *pref) {
  (void)curr;
  (void)pref;
}

SettingsProfileItem::Config SettingsWindow::getConfig() { return profiles["Default"].get()->cfg; }
