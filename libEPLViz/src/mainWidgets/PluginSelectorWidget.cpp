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

void PluginSelectorWidget::reset() {
  recording = false;
  setEnabled(true);

  // Reenable all boxes
  /*for (int i = 0; i < count(); i++) {
    QListWidgetItem *qwi = item(i);

    QCheckBox *box = qobject_cast<QCheckBox *>(itemWidget(qwi));

    // Check if object is valid
    if (!box)
      continue;

    box->setEnabled(true);
  }*/
}

void PluginSelectorWidget::addItem(QString plugin) {
  QListWidgetItem *it  = new QListWidgetItem(this);
  QCheckBox *      box = new QCheckBox(plugin, this);

  box->setToolTip(plugin);

  plugins.append(plugin);

  // QObject::connect(box, SIGNAL(stateChanged(int)), this, SLOT(changeState(int)));

  setItemWidget(it, box);
}

void PluginSelectorWidget::setMainWindow(MainWindow *mw) { main = mw; }

void PluginSelectorWidget::addPlugins(QMap<QString, QString> map) {
  // Discard the event as adding plugins during runtime is unsafe
  if (recording)
    return;

  QMapIterator<QString, QString> i(map);
  QString pluginPath = QString();

  // Try to obtain the user plugin directory
  if (main) {
    pluginPath = QString::fromStdString(main->getSettingsWin()->getConfig().pythonPluginsDir);

    QDir pluginFolder(pluginPath);

    // Check if plugin folder exists
    if (!pluginFolder.exists())
      // Try to create the full path to the folder
      if (!pluginFolder.mkpath(".")) {
        pluginPath = QString(); // Reset plugin path
      }
  }

  if (pluginPath == QString()) {

    QMessageBox::critical(0,
                          "Error",
                          tr("Could not create plugin folder at '%1'. "
                             "Ensure you have access to the folder and try again.")
                                .arg(QString::fromStdString(main->getSettingsWin()->getConfig().pythonPluginsDir)));
    return;
  }

  // Add the plugin path to the python runtime paths
  EPL_DataCollect::plugins::PythonInit::addPath(pluginPath.toStdString());

  while (i.hasNext()) {
    i.next();

    QString plugin = i.key();
    QString path   = i.value();

    // Remove python file extensions
    std::regex ex("\\.pyc?$");
    plugin = QString::fromStdString(std::regex_replace(plugin.toStdString(), ex, ""));

    // Check if the plugin is already listed
    if (plugins.contains(plugin))
      continue;

    QString newPath = pluginPath;

    if (!newPath.endsWith('/')) {
      newPath += "/";
    }

    newPath += plugin;

    if (newPath != path) {
      if (QFile::exists(newPath)) {
        QMessageBox  msg(this);
        QPushButton *owBtn     = msg.addButton("Overwrite", QMessageBox::ActionRole);
        QPushButton *rnBtn     = msg.addButton("Rename", QMessageBox::ActionRole);
        QPushButton *cancelBtn = msg.addButton("Cancel", QMessageBox::ActionRole);

        msg.setText("The file '" + plugin + "' is already present in the plugin folder.");
        msg.setInformativeText("Would you like to overwrite or rename the old file?");
        msg.exec();

        QPushButton *clicked = dynamic_cast<QPushButton *>(msg.clickedButton());

        if (clicked == owBtn) {
          // Remove old file
          QFile::remove(newPath);
        } else if (clicked == rnBtn) {
          // Rename the old file
          QString  renamed = newPath + "0";
          uint16_t counter = 1;

          // Sensibly try to find a fitting name for the renamed file
          while (QFile::exists(renamed) || counter == UINT16_MAX) {
            renamed = newPath + QString::number(counter);
            counter++;
          }

          if (counter == UINT16_MAX) {
            QFile::remove(renamed);
          } else {
            QFile::rename(newPath, renamed);
          }
        } else if (clicked == cancelBtn) {
          // Cancelling skips this plugin
          continue;
        }
      }

      qDebug() << "Trying to move " << path << " to " << newPath;

      if (!QFile::copy(path, newPath)) {

        QMessageBox::critical(0,
                              "Error",
                              tr("Unable to create file '%1'. "
                                 "Please ensure that you have the required permissions to access the plugin folder.")
                                    .arg(newPath));
        return;
      }

      qDebug() << "Moved file to " << newPath;
    }

    addItem(plugin);
  }
}

// TODO: Reimplement this once the backend has the capability to disable plugins
/*void PluginSelectorWidget::changeState(int state) {
  if (recording) {
    QCheckBox *sender = qobject_cast<QCheckBox *>(QObject::sender());

    // Check if object is valid and box is unchecked
    if (!sender || state != 0)
      return;

    std::string name = sender->toolTip().toStdString();

    sender->setEnabled(false);

    // Plugins can only be disabled while a recording is active
    if (!pluginManager->removePlugin(name)) {
      QMessageBox::critical(0, "Error", tr("Could not disable the plugin '%1'!").arg(sender->toolTip()));
    }
  }
}*/

void PluginSelectorWidget::loadPlugins(EPL_DataCollect::CaptureInstance *ci) {
  if (recording)
    return;

  pluginManager = ci->getPluginManager();

  for (int i = 0; i < count(); i++) {
    QListWidgetItem *qwi = item(i);

    QCheckBox *box = qobject_cast<QCheckBox *>(itemWidget(qwi));

    // Check if object is valid
    if (!box)
      continue;

    if (box->isChecked()) {
      // Plugin is enabled, load it in the backend
      pluginManager->addPlugin(std::make_shared<EPL_DataCollect::plugins::PythonPlugin>(plugins[i].toStdString()));
    }
    // TODO: Readd
    /*else {
      // Plugin is not enabled, disable the checkbox to prevent activation during recording
      box->setEnabled(false);
    }*/
  }

  this->setEnabled(false);
  recording = true;
}
