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
 * \file currentodmodel.cpp
 */
#include "currentodmodel.hpp"
#include "OD.hpp"
#include <QString>
using namespace EPL_Viz;
using namespace EPL_DataCollect;

CurrentODModel::CurrentODModel(QMainWindow *window) : BaseModel() {
  QTableView *view = window->findChild<QTableView *>("curNodeODView");
  view->setModel(this);
  view->verticalHeader()->hide();
  needUpdate = true;
}

void CurrentODModel::init() {}

QModelIndex CurrentODModel::parent(const QModelIndex &index) const {
  if (odEntries.value(static_cast<uint8_t>( convertRow.find(index.row())->first))->hasSubIndex())
    return CurrentODModel::index(index.row(), 0, index);
  else
    return QModelIndex();
}

QModelIndex CurrentODModel::index(int row, int column, const QModelIndex &parent) const {
  (void)parent;
  return createIndex(row, column, odEntries.value(static_cast<uint8_t>(convertRow.find(row)->first)).get());
}


int CurrentODModel::rowCount(const QModelIndex &parent) const {
  (void)parent;
  return static_cast<int>(convertRow.size());
}

int CurrentODModel::columnCount(const QModelIndex &parent) const {
  (void)parent;
  return 2;
}

QVariant CurrentODModel::data(const QModelIndex &index, int role) const {
  // TODO
  if (role == Qt::DisplayRole) {

    std::shared_ptr<CurODModelItem> entry = odEntries.value(static_cast<uint8_t>(convertRow.find(index.row())->first));
    switch (index.column()) {
      case 0: return QVariant(index.column());
      case 1: return QVariant("empty");
      default: return QVariant("This shouldn't have happened");
    }
  }
  return QVariant();
}

QVariant CurrentODModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role == Qt::DisplayRole) {
    if (orientation == Qt::Horizontal) {
      switch (section) {
        case 0: return QString("Index");
        case 1: return QString("Value");
      }
    }
  }
  return QVariant();
}

void CurrentODModel::update(EPL_DataCollect::Cycle *cycle) {
  if (!needUpdate)
    return;
  // Create Model
  Node *n = cycle->getNode(node);
  if (n == nullptr) {
    qDebug() << "Node in CurrentODModel not really set";
    return;
  }
  beginResetModel();
  OD * od      = n->getOD();
  auto entries = od->getWrittenValues();
  int  counter = 0;

  for (auto i : entries) {
    ODEntry *                       entry = od->getEntry(i);
    std::shared_ptr<CurODModelItem> item;
    if (entry->getArraySize() >= 0) {
      item = std::make_shared<CurODModelItem>(i, true);
      for (uint8_t subI = 0; subI < entry->getArraySize(); ++subI) {
        item->setSubIndex(subI, QString::fromStdString(entry->toString(static_cast<uint8_t>(i))));
        convertRow[counter++] = std::make_pair(i, subI);
      }
    } else {
      item = std::make_shared<CurODModelItem>(i, false);
      item->setSubIndex(0, QString::fromStdString(entry->toString(static_cast<uint8_t>(i))));
      convertRow[counter++] = std::make_pair(i, 0);
    }
    odEntries.insert(i, item);
  }
  endResetModel();
  needUpdate = false;
  qDebug() << "Finished updating currentodmodel";
}

void CurrentODModel::updateNext() { needUpdate = true; }

void CurrentODModel::changeNode(uint8_t n) { node = n; }
