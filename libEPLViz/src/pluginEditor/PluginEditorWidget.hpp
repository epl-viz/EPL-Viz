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
 */

#pragma once
#include "EPLVizDefines.hpp"
#if USE_KTEXTEDITOR
#include <KTextEditor/Document>
#include <KTextEditor/Editor>
#include <KTextEditor/View>
#endif
#include <QFile>
#include <QFileInfo>
#include <QGridLayout>
#include <QMap>
#include <QMessageBox>
#include <QUrl>
#include <QWidget>

class PluginEditorWidget : public QWidget {
  Q_OBJECT

 private:
#if USE_KTEXTEDITOR
  KTextEditor::Document *doc  = nullptr;
  KTextEditor::View *    view = nullptr;
#else
  QMap<QString, QString> files;
#endif
  QGridLayout *layout = nullptr;

  bool showStatusBar = false;

 public:
  PluginEditorWidget(QWidget *parent = nullptr);
  ~PluginEditorWidget();

 private:
  void loadDocument(QUrl fileName = QUrl());
  void createWidget();

 public:
  void closeDocument(QString name);

 signals:
  void nameChanged(QString name);
  void urlChanged(QString url);
  void modifiedChanged(bool modified);
  void pluginsSaved(QMap<QString, QString> savedPlugins);
  void cleanupDone();

 private slots:
  void modified();
  void nameChange();
  void urlChange();

 public slots:
  void statusBarToggled(bool enabled);
  void configEditor();

  void selectPlugin(QString plugin);

  void openFile(QUrl file);
  void cleanUp();
  void save();
  void saveAs();
  void newFile();
};
