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

#include "PluginsWindow.hpp"
#include "MainWindow.hpp"
#include "ui_pluginswindow.h"
#include <QCloseEvent>

using namespace EPL_Viz;

PluginsWindow *PluginsWindow::create(MainWindow *mw) {
  if (instance) {
    // The window already exists, focus it
    QApplication::setActiveWindow(instance);
    return nullptr;
  } else {
    // The window needs to be created
    instance = new PluginsWindow(mw);
    return instance;
  }
}

PluginsWindow::PluginsWindow(MainWindow *mw) : QMainWindow(mw), ui(new Ui::PluginsWindow) {
  ui->setupUi(this);
  setAttribute(Qt::WA_QuitOnClose); // Quit the application if this is the last window

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

PluginsWindow::~PluginsWindow() {
  instance = nullptr;
  delete ui;
}

PluginEditorWidget *PluginsWindow::getEditor() { return ui->editor; }

void PluginsWindow::closeEvent(QCloseEvent *event) {
  emit cleanUp();
  event->accept(); // The application will now quit if this is the last window
  delete this;     // Delete this window
}

void PluginsWindow::open() {
  QUrl file = QFileDialog::getOpenFileUrl(0, "Open Python file", QString(), "Python files (*.py);;All Files (*)");

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

    ui->editor->closeDocument(name);
    delete ui->pluginList->takeItem(ui->pluginList->row(item));
  }
}

PluginsWindow *PluginsWindow::instance;
