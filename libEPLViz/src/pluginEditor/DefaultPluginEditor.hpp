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
 * \file DefaultPluginEditor.hpp
 */

#pragma once

#include "PluginEditorBase.hpp"
#include <QFile>
#include <QFileDialog>
#include <QFileInfo>
#include <QGridLayout>
#include <QMap>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QString>
#include <QTextStream>
#include <QUrl>
#include <QWidget>

class DefaultPluginEditor : public PluginEditorBase {

 private:
  QMap<QString, QString> files;    // File-Name Keys, File-Path as values
  QMap<QString, QString> contents; // File-Name Keys, Current content of the file in workspace as values

  QList<QString> dirtyFiles; // Files that have unsaved changes

  QString         current;          // Current file name
  QPlainTextEdit *widget = nullptr; // The text edit widget

  QGridLayout *layout = nullptr;

  size_t newCounter = 1;

 public:
  DefaultPluginEditor(PluginEditorWidget *parent);
  ~DefaultPluginEditor();

 public:
  void updateStatusBar(bool enabled) override;
  void openConfig() override;

  void selectDocument(QString doc) override;
  void closeDocument(QString name) override;
  void openDocument(QUrl file) override;
  void newDocument() override;

  void cleanUp() override;
  void save() override;
  void saveAs() override;

 private:
  void loadDocument(QUrl fileName = QUrl());
  void createWidget();
  void writeToFile(QString filePath);

 private slots:
  void modified() override;
  void nameChange() override;
  void urlChange() override;
};