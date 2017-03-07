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
#include <QTreeView>
using namespace EPL_Viz;
using namespace EPL_DataCollect;

CurrentODModel::CurrentODModel(QMainWindow *window) : BaseModel() {

  odEntries.insert(0, std::make_shared<CurODModelItem>(0, false, false, QString("first node")));
  convertRow[0]                          = std::make_pair(0, UINT16_MAX);
  std::shared_ptr<CurODModelItem> parent = std::make_shared<CurODModelItem>(1, true, false, QString("first node"));
  convertRow[1]                          = std::make_pair(1, UINT16_MAX);
  odEntries.insert(1, parent);
  parent->setSubIndex(0, std::make_shared<CurODModelItem>(1, false, true, QString("first node")));
  convertRow[2] = std::make_pair(1, 0);

  QTreeView *view = window->findChild<QTreeView *>("curNodeODView");
  view->setModel(this);
  needUpdate = true;
}

void CurrentODModel::init() {}

QModelIndex CurrentODModel::parent(const QModelIndex &index) const {
  auto it = convertRow.value(index.row());
  // auto odindex = it->second.first;
  if (it.second == UINT16_MAX) {
    return QModelIndex();
  } else {
    auto row = index.row();
    auto sub = it.second;
    auto i   = row - sub;
    return createIndex(i, index.column(), getItem(index)->getParent().get());
  }
}

std::shared_ptr<CurODModelItem> CurrentODModel::getItem(const QModelIndex &index) const { return getItem(index.row()); }

std::shared_ptr<CurODModelItem> CurrentODModel::getItem(int row) const {
  auto it = convertRow.value(row);

  if (it.second == UINT16_MAX) {
    auto i     = it.first;
    auto entry = odEntries.value(i);
    return entry;
  } else {
    auto i     = it.first;
    auto sub   = it.second;
    auto entry = odEntries.value(i)->getSubindex(sub);
    return entry;
  }
}

QModelIndex CurrentODModel::index(int row, int column, const QModelIndex &parent) const {
  (void)parent;
  auto it      = convertRow.find(row);
  auto odindex = it->first;
  // qDebug() << "row: " << QString::number(row) << " od: " << QString::number(odindex);
  if (odEntries.contains(odindex))
    return createIndex(row, column, getItem(row).get());
  else
    return QModelIndex();
}


int CurrentODModel::rowCount(const QModelIndex &parent) const {
  (void)parent;
  auto size = convertRow.size();
  // qDebug() << "showing: " << QString::number(size);
  return static_cast<int>(size);
}

int CurrentODModel::columnCount(const QModelIndex &parent) const {
  (void)parent;
  return 2;
}

QVariant CurrentODModel::data(const QModelIndex &index, int role) const {
  // TODO
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole) {
    switch (index.column()) {
      case 0: return QString::fromStdString("row: ") + QString::number(index.row());
      case 1: return getItem(index)->getData();
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
  (void)cycle;
  beginResetModel();
  /*
  // Create Model
  Node *n = cycle->getNode(node);
  if (n == nullptr) {
    qDebug() << "Node in CurrentODModel not really set";
    return;
  }
  OD * od      = n->getOD();
  auto entries = od->getWrittenValues();
  int  counter = 0;

  for (auto i : entries) {
    ODEntry *                       entry = od->getEntry(i);
    std::shared_ptr<CurODModelItem> item;
    if (entry->getArraySize() >= 0) {
      item = std::make_shared<CurODModelItem>(i, true, false, "Subindices");
      convertRow[counter++] = std::make_pair(i, UINT16_MAX);
      for (uint16_t subI = 0; subI < entry->getArraySize(); ++subI) {
        // TODO Unterschied toString sub und nicht sub
        std::shared_ptr<CurODModelItem> sub = std::make_shared<CurODModelItem>(subI, false, true,
  QString::fromStdString(entry->toString(i)), item);
        item->setSubIndex(subI, sub);
        convertRow[counter++] = std::make_pair(i, subI);
      }
    } else {
      item = std::make_shared<CurODModelItem>(i, false, false, QString::fromStdString(entry->toString(0)), nullptr);
      convertRow[counter++] = std::make_pair(i, UINT16_MAX);
    }
    odEntries.insert(i, item);
  }
  */
  // debug start


  endResetModel();
  // needUpdate = false;
  qDebug() << "Finished updating currentodmodel";
}

void CurrentODModel::updateNext() { needUpdate = true; }

void CurrentODModel::changeNode(uint8_t n) { node = n; }
