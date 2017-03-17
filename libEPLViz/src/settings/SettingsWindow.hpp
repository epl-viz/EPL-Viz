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

#pragma once

#include "ProfileManager.hpp"
#include "SettingsProfileItem.hpp"
#include <QDialog>
#include <QListWidget>
#include <memory>
#include <unordered_map>

namespace Ui {
class SettingsWindow;
}

namespace EPL_Viz {

class MainWindow;

class SettingsWindow : public QDialog {
  Q_OBJECT
 private:
  Ui::SettingsWindow *ui;
  MainWindow *        mainWindow;
  ProfileManager *    conf;

  std::unordered_map<std::string, std::shared_ptr<SettingsProfileItem>> profiles;

  std::string currentProfile = "Default";

  void loadConfig();
  void saveConfig();
  void saveIntoProfiles();
  void updateView(bool updateNodes = true);

  SettingsProfileItem::Config startCFG;

 public slots:
  void applyOn(EPL_DataCollect::CaptureInstance *ci);
  void apply();
  void reset();

  void newProfile();
  void deleteProfile();
  void newNode();
  void deleteNode();

  void profChange(QListWidgetItem *curr, QListWidgetItem *pref);
  void nodeChange(QListWidgetItem *curr, QListWidgetItem *pref);

 public:
  explicit SettingsWindow(QWidget *parent, ProfileManager *settings);
  ~SettingsWindow();

  SettingsProfileItem::Config getConfig();
};
}
