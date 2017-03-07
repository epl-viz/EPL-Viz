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
 * \file pyhonlogmodel.cpp
 */
#include "pythonlogmodel.hpp"
#include "EPLEnums.h"
#include "mainwindow.hpp"
using namespace EPL_Viz;
using namespace EPL_DataCollect;

PythonLogModel::PythonLogModel(MainWindow *window) : BaseModel() {
  qDebug() << "Updating PythonLogModel";
  log              = window->getCaptureInstance()->getEventLog();
  QTableView *view = window->findChild<QTableView *>("pythonLogView");
  view->setModel(this);
  view->verticalHeader()->hide();
}

void PythonLogModel::init() { appid = log->getAppID(); }

void PythonLogModel::update(Cycle *cycle) {
  (void)cycle;
  events                             = log->getAllEvents();
  std::vector<EventBase *> newEvents = log->pollEvents(appid);
  insertRows(static_cast<int>(events.size()), static_cast<int>(newEvents.size()));
}

int PythonLogModel::rowCount(const QModelIndex &parent) const {
  (void)parent;
  return static_cast<int>(events.size());
}

int PythonLogModel::columnCount(const QModelIndex &parent) const {
  (void)parent;
  return 5;
}

QVariant PythonLogModel::data(const QModelIndex &index, int role) const {
  if (role == Qt::DisplayRole) {
    EventBase *ev = events[static_cast<uint8_t>(index.row())];
    switch (index.column()) {
      case 0:
        uint32_t start;
        ev->getCycleRange(&start);
        return QVariant(start);
      case 1: return QString::fromStdString(ev->getTypeAsString());
      case 2: return QString::fromStdString(ev->getPluginID());
      case 3: return QString::fromStdString(ev->getName());
      case 4: return QString::fromStdString(ev->getDescription());
    }
  }
  return QVariant();
}

QVariant PythonLogModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role == Qt::DisplayRole) {
    if (orientation == Qt::Horizontal) {
      switch (section) {
        case 0: return QString("Cycle");
        case 1: return QString("Type");
        case 2: return QString("Plugin ID");
        case 3: return QString("Name");
        case 4: return QString("Description");
      }
    }
  }
  return QVariant();
}

bool PythonLogModel::insertRows(int row, int count, const QModelIndex &parent) {
  beginInsertRows(parent, row, row + count - 1);
  endInsertRows();
  return true;
}
