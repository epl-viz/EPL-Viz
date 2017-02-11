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
#include "EPLVizDefines.hpp"
#include "guistate.hpp"
#include "profilemanager.hpp"
#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

 private:
  Ui::MainWindow *         ui;
  EPL_Viz::ProfileManager *profileManager;
  EPL_Viz::GUIState        machineState;
  int                      curCycle;

 private:
  /*!
   * \brief main loop
   */
  mockable void loop();

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
  mockable bool changeCycle(int cycle);
  /*!
   * \brief Returns the current state
   * \return the current state
   */
  mockable EPL_Viz::GUIState getState();

  /*!
   * \brief Starts the main loop
   * \return true if it succeeded
   */
  mockable bool startLoop();

 public slots:
  void setFullscreen(bool makeFullscreen);
  void openPluginEditor();
  void openInterfacePicker();
  void save();
  void saveAs();
  void open();
};
