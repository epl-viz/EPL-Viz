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
 * \file BaseModel.cpp
 */

#include "BaseModel.hpp"
#include "DefaultFilter.hpp"
#include "MainWindow.hpp"

using namespace EPL_Viz;
using namespace EPL_DataCollect;
using namespace EPL_DataCollect::constants;
using namespace plugins;

QLinkedList<BaseModel *> BaseModel::registeredModels;

BaseModel::BaseModel(MainWindow *mw, QWidget *widget) {
  (void)widget;

  reg(this);
  mainWindow = mw;
}

BaseModel::~BaseModel() { dereg(this); }

MainWindow *BaseModel::getMainWindow() { return mainWindow; }

void BaseModel::updateAll(MainWindow *mw, CaptureInstance *instance) {
  if (instance == nullptr) {
    qDebug() << "CaptureInstance is a nullptr";
    return;
  }

  uint32_t oldCycleNum = cycle->getCycleNum();

  // Get Cycle
  if (mw->getCycleNum() == UINT32_MAX || mw->getCycleNum() != oldCycleNum)
    cycle.updateCycle(instance, mw->getCycleNum());

  uint32_t newCycleNum = cycle->getCycleNum();

  // Prevent unnecessary updates
  if (newCycleNum == UINT32_MAX || newCycleNum == oldCycleNum)
    return;

  // Filter
  CycleStorageBase *b   = cycle->getCycleStorage(EPL_DC_PLUGIN_VIEW_FILTERS_CSID);
  CSViewFilters *   csF = dynamic_cast<CSViewFilters *>(b);

  if (!b)
    qDebug() << "Not registered " << EPL_DC_PLUGIN_VIEW_FILTERS_CSID.c_str();

  if (csF == nullptr) {
    qDebug() << "No filters set";
  } else {
    auto filters = csF->getFilters();
    mw->setFilters(filters);
  }

  GUIState state = mw->getState();

  EventLog *log = instance->getEventLog();

  // Check if appID has been set already
  if (appID == UINT32_MAX) {
    // Retrieve appID for the BaseModel
    appID = log->getAppID();
    qDebug() << "[BaseModel] Retrieved new appID " << QString::number(appID);
  }

  // Poll all events
  auto events = log->pollEvents(appID);

  for (auto event : events) {
    switch (event->getType()) {
      case EvType::VIEW_ENDCAP:
        // Check if the GUI is running
        if (state == GUIState::RECORDING || state == GUIState::PLAYING) {
          // Pause recording/playing
          mw->changeState(GUIState::PAUSED); // TODO: Add configuration option for pausing playing
        }
        break;
      case EvType::VIEW_STARTCAP:
        // Check if the GUI is paused
        if (state == GUIState::PAUSED) {
          mw->changeState(GUIState::PLAYING);
        }
        break;
      default: break;
    }
  }

  // If the recording is paused, do not update models
  if (state == GUIState::PAUSED)
    return;

  // Update models
  for (auto &i : registeredModels) {
    //    qDebug() << "[" << cycle->getCycleNum() << "] Updating " << i->getName();
    i->update(cycle);
    //    qDebug() << "[" << cycle->getCycleNum() << "] DONE     " << i->getName();
  }

  uint32_t cycleNum = cycle->getCycleNum();
  if (cycleNum > mw->getMaxCycle())
    mw->setMaxCycle(cycleNum);
}

ProtectedCycle &BaseModel::getCurrentCycle() { return cycle; }

void BaseModel::initAll() {
  appID = UINT32_MAX; // Initialize appID with a dummy value

  for (auto &i : registeredModels) {
    //    qDebug() << "[INIT] Updating " << i->getName();
    i->init();
    //    qDebug() << "[INIT] DONE     " << i->getName();
  }
}

void BaseModel::reg(BaseModel *model) {
  if (!registeredModels.contains(model)) {
    qDebug() << "Registered a model";
    registeredModels.append(model);
  } else
    throw std::runtime_error("Cannot add a model twice!");
}

void BaseModel::dereg(BaseModel *model) {
  registeredModels.removeOne(model);
  qDebug() << "Deregistered a model";
}

bool BaseModel::operator==(const BaseModel &other) { return this == &other; }

uint32_t       BaseModel::appID;
ProtectedCycle BaseModel::cycle;
