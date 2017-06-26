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
 * \file PluginsWindows.cpp
 */

#include "defines.hpp"
#include "PluginsWindow.hpp"
#include "MainWindow.hpp"
#include "ui_pluginswindow.h"
#include <QCloseEvent>

#if __cplusplus <= 201402L
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

#if defined(WIN32) || !defined(_WIN32) || !defined(__WIN32)
#include <windows.h>
#endif


using namespace EPL_Viz;
using namespace EPL_DataCollect::constants;

PluginsWindow::PluginsWindow(MainWindow *mw) : QMainWindow(mw), ui(new Ui::PluginsWindow) {
  ui->setupUi(this);

  connect(this, SIGNAL(closed()), ui->editor, SLOT(cleanUp()));
  connect(this, SIGNAL(closed()), ui->pluginList, SLOT(cleanUp()));
}

PluginsWindow::~PluginsWindow() { delete ui; }

void PluginsWindow::loadPlugins(MainWindow *mw) {
  QString pluginPath   = QString::fromStdString(mw->getSettingsWin()->getConfig().pythonPluginsDir);
  QDir    pluginFolder = QDir(pluginPath);

  if (pluginFolder.exists()) {
    // Load plugins in the plugin folder
    QStringListIterator plugins(pluginFolder.entryList(QDir::Files));

    // Open plugins in the plugin folder
    while (plugins.hasNext()) {
      emit fileOpened(QUrl::fromLocalFile(pluginPath + "/" + plugins.next()));
    }
  }
}

PluginEditorWidget *PluginsWindow::getEditor() { return ui->editor; }

void PluginsWindow::closeEvent(QCloseEvent *event) {
  (void)event;
  emit closed();
}

void PluginsWindow::open() {
  char *      appImageDir = nullptr;
  std::string windowsPath;
  fs::path    pOpen;
#if !defined(WIN32) && !defined(_WIN32) && !defined(__WIN32)
  appImageDir = getenv("APPDIR");
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32)
  TCHAR exePathCSTR[MAX_PATH];
  GetModuleFileName(NULL, exePathCSTR, MAX_PATH);
  fs::path tempExePath(exePathCSTR);
  windowsPath = tempExePath.remove_filename().remove_filename().string();
#endif
  std::string defaultPathSTR = EPL_DC_INSTALL_PREFIX + "/share/eplViz/plugins/samples";
  if (appImageDir) {
    defaultPathSTR = std::string(appImageDir) + "/usr/share/eplViz/plugins/samples";
  } else if (!windowsPath.empty()) {
    defaultPathSTR = windowsPath + "/share/eplViz/plugins/samples";
  }

  pOpen = defaultPathSTR;

  QString defaultPath = pOpen.string().c_str();

  QUrl file = QFileDialog::getOpenFileUrl(
        0, "Open Python file", QUrl::fromLocalFile(defaultPath), "Python files (*.py);;All Files (*)");

  if (file == QUrl())
    return;

  emit fileOpened(file);
}

void PluginsWindow::closeFile() {
  QListWidgetItem *item = ui->pluginList->currentItem();
  if (item) {
    QString name = item->text();

    if (name.endsWith('*'))
      name.chop(1);

    ui->editor->closeFile(name);
    delete ui->pluginList->takeItem(ui->pluginList->row(item));
  }
}
