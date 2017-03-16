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
 * \file PluginSelectorWidget.cpp
 */

#include "PluginSelectorWidget.hpp"
#include "PythonInit.hpp"
#include <algorithm>
#include <regex>

using namespace EPL_Viz;

PluginSelectorWidget::PluginSelectorWidget(QWidget *parent) : QListWidget(parent) {}

void PluginSelectorWidget::addItem(QString plugin) {
  QListWidgetItem *it  = new QListWidgetItem(this);
  QCheckBox *      box = new QCheckBox(plugin, this);


  QObject::connect(box, SIGNAL(stateChanged(int)), this, SLOT(changeState(int)));

  setItemWidget(it, box);
}

void PluginSelectorWidget::setMainWindow(MainWindow *mw) { main = mw; }

void PluginSelectorWidget::addPlugins(QMap<QString, QString> map) {
  QMapIterator<QString, QString> i(map);
  QString pluginPath = QDir::currentPath();

  if (main)
    pluginPath = QString::fromStdString(main->getSettingsWin()->getConfig().pythonPluginsDir);

  EPL_DataCollect::plugins::PythonInit::addPath(pluginPath.toStdString());

  while (i.hasNext()) {
    i.next();

    QString plugin = i.key();
    QString path   = i.value();

    QString newPath = pluginPath + "/" + plugin;

    if (newPath != path) {
      if (QFile::exists(newPath)) {
        qDebug() << "File already exists! Removing it...";
        QFile::remove(newPath);
        // TODO: Add a dialog to warn for overwriting the file
      }

      qDebug() << "Trying to move " << path << " to " << newPath;

      if (!QFile::copy(path, newPath)) {
        qDebug() << "Failed to move file!";
        continue;
      }

      qDebug() << "Moved file to " << newPath;
    }

    // Remove python file extensions
    std::regex ex("\\.pyc?$");
    plugin = QString::fromStdString(std::regex_replace(plugin.toStdString(), ex, ""));

    addItem(plugin);
  }
}


void PluginSelectorWidget::changeState(int state) {
  if (recording) {
    QCheckBox *sender = qobject_cast<QCheckBox *>(QObject::sender());

    // Check if object is valid and box is unchecked
    if (!sender || state != 0)
      return;

    std::string name = sender->text().toStdString();

    // Remove invalid mnemonics
    name.erase(std::remove(name.begin(), name.end(), '&'), name.end());

    sender->setEnabled(false);

    // Plugins can only be disabled while a recording is active
    if (!pluginManager->removePlugin(name))
      qDebug() << "Could not disable " << QString::fromStdString(name);
  }
}

void PluginSelectorWidget::loadPlugins(EPL_DataCollect::CaptureInstance *ci) {
  qDebug() << "Received plugin load signal!";
  pluginManager = ci->getPluginManager();

  for (int i = 0; i < count(); i++) {
    QListWidgetItem *qwi = item(i);

    QCheckBox *box = qobject_cast<QCheckBox *>(itemWidget(qwi));

    // Check if object is valid
    if (!box)
      continue;

    if (box->isChecked()) {
      // Plugin is enabled, load it in the backend
      std::string name = box->text().toStdString();

      // Remove invalid mnemonics
      name.erase(std::remove(name.begin(), name.end(), '&'), name.end());

      pluginManager->addPlugin(std::make_shared<EPL_DataCollect::plugins::PythonPlugin>(name));
    } else {
      // Plugin is not enabled, disable the checkbox to prevent activation during recording
      box->setEnabled(false);
    }
  }
}

void PluginSelectorWidget::disableWidget() { setEnabled(false); }
