/* Copyright (c) 2017, EPL-Vizards
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
 * \file PluginEditorWidget.hpp
 * \brief The hosting widget for the plugin code editor widget.
 *
 * The PluginEditorWidget selects and hosts the plugin code editor.
 */

#pragma once
#include "EPLVizDefines.hpp"
#include <QDebug>
#include <QGridLayout>
#include <QMainWindow>
#include <QWidget>

class PluginEditorBase;

class PluginEditorWidget : public QWidget {
  Q_OBJECT

 private:
  PluginEditorBase *editor = nullptr;
  QGridLayout *     layout = nullptr;

 public:
  PluginEditorWidget(QWidget *parent = nullptr);
  ~PluginEditorWidget();

 signals:
  void nameChanged(QString newName);
  void urlChanged(QString newUrl);
  void modifiedStateChanged(bool modifiedState);
  void filesSaved(QMap<QString, QString> savedFiles);

 private slots:
  void modifiedChange(bool modified);
  void nameChange(QString newName);
  void urlChange(QString newUrl);
  void filesSave(QMap<QString, QString> savedFiles);

 public slots:
  void toggleStatusBar(bool enabled);
  void configureEditor();

  void selectFile(QString file);

  void openFile(QUrl file);
  void cleanUp();
  void save();
  void saveAs();
  void newFile();
  void closeFile(QString name);
};
