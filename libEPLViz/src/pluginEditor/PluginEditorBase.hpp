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
 * \file PluginEditorBase.hpp
 */

#pragma once

#include "PluginEditorWidget.hpp"
#include <QWidget>

class PluginEditorBase : public QWidget {
  Q_OBJECT

 public:
  PluginEditorBase(PluginEditorWidget *parent) : QWidget(parent) {}
  ~PluginEditorBase() = default;

 public:
  virtual void updateStatusBar(bool enabled) = 0;
  virtual void openConfig()                  = 0;

  virtual void selectDocument(QString doc) = 0;
  virtual void closeDocument(QString name) = 0;
  virtual void openDocument(QUrl file)     = 0;
  virtual void newDocument()               = 0;

  virtual void cleanUp() = 0;
  virtual void save()    = 0;
  virtual void saveAs()  = 0;

 signals:
  void nameChanged(QString name);
  void urlChanged(QString url);
  void modifiedChanged(bool modified);
  void pluginsSaved(QMap<QString, QString> savedPlugins);

 private slots:
  virtual void modified()   = 0;
  virtual void nameChange() = 0;
  virtual void urlChange()  = 0;
};
