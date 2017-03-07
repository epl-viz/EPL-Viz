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
 * \file mainwindow.hpp
 */
#pragma once
#include "CaptureInstance.hpp"
#include "EPLVizDefines.hpp"
#include "basemodel.hpp"
#include "currentodmodel.hpp"
#include "guistate.hpp"
#include "modelthread.hpp"
#include "nodewidget.hpp"
#include "packethistorymodel.hpp"
#include "profilemanager.hpp"
#include "pythonlogmodel.hpp"
#include "qwtplotmodel.hpp"
#include "settingswindow.hpp"
#include <QAction>
#include <QDebug>
#include <QLabel>
#include <QLinkedList>
#include <QMainWindow>
#include <QThread>
#include <QToolBar>
#include <QToolButton>
#include <iostream>
#include <vector>

namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow {
  Q_OBJECT

 private:
  Ui::MainWindow *                                  ui;
  EPL_Viz::ProfileManager *                         profileManager;
  EPL_Viz::GUIState                                 machineState;
  uint32_t                                          curCycle;
  EPL_Viz::ModelThread *                            modelThread;
  QLinkedList<EPL_Viz::BaseModel *>                 models;
  std::unique_ptr<EPL_DataCollect::CaptureInstance> captureInstance;
  QString                                           interface;
  std::string                                       file;
  SettingsWindow *                                  settingsWin;

 public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
  /*!
   * \brief Changes the cycle to the one before the timestamp
   * Tries to change the time, if the current state does not allow it, nothing happens.
   * \param t Time in seconds
   * \return whether or not the change was a success
   */
  mockable bool changeTime(double t);
  /*!
   * \brief Changes the cycle
   * Tries to change the cycle, if the current state does not allow it, nothing happens.
   * \param cycle Cycle number
   * \return whether or not the  change was a success
   */
  mockable bool changeCycle(uint32_t cycle);
  mockable void changeState(EPL_Viz::GUIState nState);
  /*!
   * \brief Returns the current state
   * \return the current state
   */
  mockable EPL_Viz::GUIState getState();

  SettingsWindow *getSettingsWin();

  mockable uint32_t getCycleNum();
  /**
   * @brief getCaptureInstance
   * Don't save the captureinstance. Cannot guarantee the lifetime of the pointer.
   * @return Raw captureinstance pointer
   */
  EPL_DataCollect::CaptureInstance *getCaptureInstance();

  QWidget *getNetworkGraph();

  static void fixQToolButtons(std::vector<QToolButton *> &btns);
  static void fixQToolButtons(std::vector<QAction *> &actions, QToolBar *bar);

 protected:
  void closeEvent(QCloseEvent *event) override;

 private:
  void createModels();
  void destroyModels();
  bool event(QEvent *event) override;
  void config();
  bool curODWidgetUpdateData(QTreeWidgetItem *item, QString newData);

 public slots:
  void setFullscreen(bool makeFullscreen);
  void openPluginEditor();
  void openInterfacePicker();
  void openSettings();
  void save();
  void saveAs();
  void open();
  void handleResults(const QString &);
  void startRecording();
  void stopRecording();
  void addNode(EPL_DataCollect::Node *n);
  void externalUpdate(EPL_DataCollect::Cycle *cycle, int node);

 signals:
  void operate(const QString &);
  void close();
  void cycleChanged();
  void recordingStarted(EPL_DataCollect::CaptureInstance *);
  void nodeAdded(uint8_t id, NodeWidget *nw);
};
