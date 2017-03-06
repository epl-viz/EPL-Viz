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
 * \file cyclecommandsmodel.cpp
 */

#include "cyclecommandsmodel.hpp"
#include "Cycle.hpp"
#include "mainwindow.hpp"
using namespace EPL_Viz;
using namespace EPL_DataCollect;

CycleCommandsModel::CycleCommandsModel(QObject *parent) : QAbstractItemModel(parent) {
  QVector<QVariant> rootData;
  rootData << "Command"
           << "Index"
           << "Value"
           << "Other";
  rootItem = new CyCoTreeItem(rootData);
  view     = static_cast<MainWindow *>(parent)->findChild<QTreeView *>("cycleCommandsView");
  view->setModel(this);
  view->setDisabled(true);
  view->setToolTip("Please start a capture or replay and select a Node");
  needUpdate = false;
}

CycleCommandsModel::~CycleCommandsModel() { delete rootItem; }

CyCoTreeItem *CycleCommandsModel::getItem(const QModelIndex &index) const {
  if (index.isValid()) {
    CyCoTreeItem *item = static_cast<CyCoTreeItem *>(index.internalPointer());
    if (item)
      return item;
  }

  return rootItem;
}

int CycleCommandsModel::rowCount(const QModelIndex &parent) const {
  CyCoTreeItem *parentItem = getItem(parent);

  return parentItem->childCount();
}

int CycleCommandsModel::columnCount(const QModelIndex &) const { return rootItem->columnCount(); }

Qt::ItemFlags CycleCommandsModel::flags(const QModelIndex &index) const {
  if (!index.isValid())
    return 0;

  return QAbstractItemModel::flags(index);
}

QModelIndex CycleCommandsModel::index(int row, int column, const QModelIndex &parent) const {
  if (parent.isValid() && parent.column() != 0)
    return QModelIndex();

  CyCoTreeItem *parentItem = getItem(parent);
  CyCoTreeItem *childItem  = parentItem->child(row);

  if (childItem)
    return createIndex(row, column, childItem);
  else
    return QModelIndex();
}

QModelIndex CycleCommandsModel::parent(const QModelIndex &index) const {
  if (!index.isValid())
    return QModelIndex();

  CyCoTreeItem *childItem  = getItem(index);
  CyCoTreeItem *parentItem = childItem->parent();

  if (parentItem == rootItem)
    return QModelIndex();

  return createIndex(parentItem->childNumber(), 0, parentItem);
}

QVariant CycleCommandsModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    return rootItem->data(section);

  return QVariant();
}

bool CycleCommandsModel::insertRows(int position, int rows, const QModelIndex &parent) {
  CyCoTreeItem *parentItem = getItem(parent);
  bool          success;
  beginInsertColumns(parent, position, position + rows - 1);
  success = parentItem->insertChildren(position, rows);
  endInsertRows();

  return success;
}

bool CycleCommandsModel::removeRows(int position, int rows, const QModelIndex &parent) {
  CyCoTreeItem *parentItem = getItem(parent);
  bool          success    = true;

  beginRemoveRows(parent, position, position + rows - 1);
  success = parentItem->removeChildren(position, rows);
  endRemoveRows();

  return success;
}

QVariant CycleCommandsModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  if (role != Qt::DisplayRole)
    return QVariant();

  CyCoTreeItem *item = getItem(index);
  return item->data(index.column());
}

void CycleCommandsModel::init() {}

void CycleCommandsModel::update(Cycle *cycle) {
  if (!needUpdate)
    return;

  std::vector<Packet> packets = cycle->getPackets();

  // Delete old tree and add new
  beginResetModel();
  rootItem->removeChildren(0, rootItem->childCount());

  for (uint32_t i = 0; i < packets.size(); ++i) {
    rootItem->insertChildren(i, 1);
    // Root Packets
    rootItem->child(i)->setData(0, QVariant("Frame " + QString::number(i)));
    // TODO set children
  }
  endResetModel();
}

void CycleCommandsModel::updateNext() { needUpdate = true; }

void CycleCommandsModel::changeNode(uint8_t newNode) {
  selectedNode = newNode;
  view->setEnabled(true);
  updateNext();
}
