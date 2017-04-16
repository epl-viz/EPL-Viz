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
 * \file ProfileManager.cpp
 */

#include "ProfileManager.hpp"
#include "MainWindow.hpp"
using namespace EPL_Viz;
using namespace profStrings;

ProfileManager::ProfileManager() {
  QString username = qgetenv("USER");
  if (username == "") {
    username = qgetenv("USERNAME");
  }
  appSettings = new QSettings("EPL-Vizards", "EPL-Viz_" + username);
  int size    = appSettings->beginReadArray(PROF_LIST);
  for (int i = 0; i < size; ++i) {
    appSettings->setArrayIndex(i);
    auto prof = appSettings->value(PROF_ITEM).toString();
    profiles.push_back(prof);
    qDebug() << "[ProfileManager] Read Profile " << prof;
  }
  appSettings->endArray();
  if (profiles.empty()) {
    qDebug() << "[ProfileManager] Adding default profile";
    profiles.push_back(DEFAULT_PROF);
  }
}

ProfileManager::~ProfileManager() {
  appSettings->sync();
  delete appSettings;
}

void ProfileManager::deleteProfile(QString profileName) {
  if (profileName == DEFAULT_PROF) {
    qDebug() << "[ProfileManager] Not removing the default profile";
  }

  for (size_t i = 0; i < profiles.size(); ++i) {
    if (profileName == profiles[i]) {
      qDebug() << "[ProfileManager] Removing profile " << profileName;
      profiles.erase(profiles.begin() + static_cast<long>(i));
      updateProfiles();
      return;
    }
  }

  qDebug() << "[ProfileManager] Could not delete Profile " << profileName << " (notFound)";
}

Profile *ProfileManager::getDefaultProfile() { return getProfile(DEFAULT_PROF); }

/*!
   * \brief Returns the profile with the given name
   *
   * If the profile does not exist, it will be created and returned
   *
   * \param profileName The name of the profile to retrieve
   * \return The profile with the given name
   */
Profile *ProfileManager::getProfile(QString profileName) {
  bool found = false;
  for (auto const &i : profiles) {
    if (i == profileName) {
      found = true;
      break;
    }
  }

  if (!found) {
    profiles.push_back(profileName);
    updateProfiles();
  }

  return new Profile(appSettings, profileName);
}

void ProfileManager::updateProfiles() {
  appSettings->beginWriteArray(PROF_LIST);
  int counter = 0;
  for (auto const &i : profiles) {
    qDebug() << "[ProfileManager] Writing profile " << i;
    appSettings->setArrayIndex(counter);
    appSettings->setValue(PROF_ITEM, i);
    ++counter;
  }
  appSettings->endArray();
}

QSettings *ProfileManager::getRawSettings() { return appSettings; }

std::vector<QString> ProfileManager::getProfiles() { return profiles; }

void ProfileManager::writeWindowSettings(MainWindow *window) {
  appSettings->beginGroup("MainWindow");
  appSettings->setValue("geometry", window->saveGeometry());
  appSettings->setValue("state", window->saveState());
  appSettings->endGroup();
}

void ProfileManager::readWindowSettings(MainWindow *window) {
  appSettings->beginGroup("MainWindow");
  window->restoreGeometry(appSettings->value("geometry").toByteArray());
  window->restoreState(appSettings->value("state").toByteArray());
  appSettings->endGroup();
}
