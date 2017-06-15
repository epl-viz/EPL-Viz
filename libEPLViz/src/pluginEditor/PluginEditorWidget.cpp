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
 * \file PluginEditorWidget.cpp
 */

#include "PluginEditorWidget.hpp"
#include "PluginEditorBase.hpp"

#if USE_KTEXTEDITOR
#include "KDEPluginEditor.hpp"
#else
#include "DefaultPluginEditor.hpp"
#endif


PluginEditorWidget::PluginEditorWidget(QWidget *parent) : QWidget(parent) {
  layout = new QGridLayout(this);
#if USE_KTEXTEDITOR
  editor = new KDEPluginEditor(this);
#else
  editor = new DefaultPluginEditor(this);
#endif
  layout->addWidget(editor);

  connect(editor, SIGNAL(nameChanged(QString)), this, SLOT(nameChange(QString)));
  connect(editor, SIGNAL(urlChanged(QString)), this, SLOT(urlChange(QString)));
  connect(editor, SIGNAL(modifiedChanged(bool)), this, SLOT(modifiedChange(bool)));
  connect(editor, SIGNAL(pluginsSaved(QMap<QString, QString>)), this, SLOT(filesSave(QMap<QString, QString>)));
}

PluginEditorWidget::~PluginEditorWidget() {
  layout->removeWidget(editor);
  delete editor;
  delete layout;
}

void PluginEditorWidget::modifiedChange(bool modified) { emit modifiedStateChanged(modified); }
void PluginEditorWidget::nameChange(QString newName) { emit nameChanged(newName); }
void PluginEditorWidget::urlChange(QString newUrl) { emit urlChanged(newUrl); }
void PluginEditorWidget::filesSave(QMap<QString, QString> savedFiles) { emit filesSaved(savedFiles); }

void PluginEditorWidget::toggleStatusBar(bool enabled) { editor->updateStatusBar(enabled); }

void PluginEditorWidget::configureEditor() { editor->openConfig(); }

void PluginEditorWidget::selectFile(QString file) { editor->selectDocument(file); }
void PluginEditorWidget::openFile(QUrl file) { editor->openDocument(file); }
void PluginEditorWidget::closeFile(QString name) { editor->closeDocument(name); }

void PluginEditorWidget::save() { editor->save(); }
void PluginEditorWidget::saveAs() { editor->saveAs(); }
void PluginEditorWidget::newFile() { editor->newDocument(); }

void PluginEditorWidget::cleanUp() { editor->cleanUp(); }
