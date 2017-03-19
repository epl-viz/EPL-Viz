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
 * \file PluginSelectorWidget.hpp
 */

#pragma once

#include "MainWindow.hpp"
#include "PythonPlugin.hpp"
#include <QCheckBox>
#include <QDir>
#include <QFile>
#include <QListWidget>
#include <QMap>
#include <QMessageBox>

namespace EPL_Viz {

class PluginSelectorWidget : public QListWidget {
  Q_OBJECT

 private:
  bool                            recording = false;
  EPL_DataCollect::PluginManager *pluginManager;
  MainWindow *                    main;

  QList<QString> plugins;

 public:
  PluginSelectorWidget(QWidget *parent = nullptr);
  ~PluginSelectorWidget()              = default;

  void setMainWindow(MainWindow *mw);

 private:
  void addItem(QString plugin);

 private slots:
  void changeState(int state);

 public slots:
  void loadPlugins(EPL_DataCollect::CaptureInstance *ci);
  void addPlugins(QMap<QString, QString> map);
  void reset();
};
}
