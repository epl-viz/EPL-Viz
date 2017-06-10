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
 * \file ModelThread.cpp
 */

#include "ModelThread.hpp"
#include "BaseModel.hpp"
#include "CaptureInstance.hpp"
#include "EPLEnum2Str.hpp"
#include "MainWindow.hpp"
#include <QDebug>

#if __cplusplus <= 201402L
#if defined(__clang__)
#define FALLTHROUGH [[clang::fallthrough]]
#else
#define FALLTHROUGH [[gcc::fallthrough]]
#endif
#else
#define FALLTHROUGH [[fallthrough]]
#endif

using namespace EPL_Viz;
using namespace EPL_DataCollect;

ModelThread::ModelThread(QObject *parent, GUIState *machineState, MainWindow *win) : QThread(parent) {
  ModelThread::state = machineState;
  window             = win;
}

ModelThread::~ModelThread() {
  // Closing thread
  quit();
  requestInterruption();
  wait();
}

ModelThread::RESULT ModelThread::update() {
  auto                     lock    = BaseModel::getUpdateLock();
  auto *                   ci      = window->getCaptureInstance();
  CaptureInstance::CIstate cistate = ci->getState();
  if (cistate == CaptureInstance::CIstate::SETUP)
    return CONTINUE;

  if (cistate != CaptureInstance::CIstate::RUNNING && cistate != CaptureInstance::CIstate::DONE) {
    qDebug() << QString::fromStdString("Stopped because ci changed state to state " + EPLEnum2Str::toStr(cistate));
    return RESET;
  }

  // Update models and if it was completed, the widgets
  if (BaseModel::updateAll(window, ci))
    return UPDATE;

  return NOTHING;
}

void ModelThread::loop() {
  while (running) {
    switch (*state) {
      case GUIState::UNINIT: yieldCurrentThread(); break;
      case GUIState::STOPPED:
      case GUIState::PLAYING:
      case GUIState::PAUSED:
      case GUIState::RECORDING: {
        switch (update()) {
          case CONTINUE: continue;
          case RESET: window->changeState(GUIState::UNINIT); break;
          case UPDATE: emit  updateCompleted(); FALLTHROUGH;
          case NOTHING: emit updateCompletedAlways(); break;
        }

        break;
      }
    }
    // TODO Constant update time or something else?
    SLEEP(milliseconds, window->getSettingsWin()->getConfig().guiThreadWaitTime);
  }
}

void ModelThread::run() {
  running = true;
  ModelThread::loop();
}

void ModelThread::stop() {
  qDebug() << "Stopping";
  running = false;
}
