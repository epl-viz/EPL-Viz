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
 * \file CurrentODModel.cpp
 */
#include "CurrentODModel.hpp"
#include "OD.hpp"
#include <QMenu>
#include <QString>
#include <QThread>
#include <algorithm>
using namespace EPL_Viz;
using namespace EPL_DataCollect;
using namespace std;

CurrentODModel::CurrentODModel(QMainWindow *window) : QAbstractItemModel(window), BaseModel() {
  //  tree->setContextMenuPolicy(Qt::CustomContextMenu);
  //  connect(tree, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));
  needUpdate = true;
}

CurrentODModel::~CurrentODModel() {
  if (root)
    delete root;
}

void CurrentODModel::init() {}



QModelIndex CurrentODModel::index(int row, int column, const QModelIndex &parent) const {
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  CurODModelItem *p;
  CurODModelItem *c;

  if (!parent.isValid())
    p = root;
  else
    p = static_cast<CurODModelItem *>(parent.internalPointer());

  c = p->child(row);
  if (c) {
    return createIndex(row, column, c);
  }

  return QModelIndex();
}

QModelIndex CurrentODModel::parent(const QModelIndex &index) const {
  if (!index.isValid())
    return QModelIndex();

  CurODModelItem *c = static_cast<CurODModelItem *>(index.internalPointer());
  CurODModelItem *p = c->parent();

  if (p == root)
    return QModelIndex();

  return createIndex(p->row(), 0, p);
}

int CurrentODModel::rowCount(const QModelIndex &parent) const {
  if (parent.column() > 0)
    return 0;

  if (!parent.isValid()) {
    if (root) {
      return root->childCount();
    } else {
      return 0;
    }
  }

  return static_cast<CurODModelItem *>(parent.internalPointer())->childCount();
}

int CurrentODModel::columnCount(const QModelIndex &parent) const {
  if (parent.column() > 0)
    return 0;

  if (!parent.isValid())
    return root->columnCount();

  return static_cast<CurODModelItem *>(parent.internalPointer())->columnCount();
}

QVariant CurrentODModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  CurODModelItem *item = static_cast<CurODModelItem *>(index.internalPointer());
  return item->data(index.column(), static_cast<Qt::ItemDataRole>(role));
}

QVariant CurrentODModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
    switch (section) {
      case 0: return QVariant("Index");
      case 1: return QVariant("Value");
      default: return QVariant("[ERROR: You should not see this]");
    }
  }

  return QVariant();
}

Qt::ItemFlags CurrentODModel::flags(const QModelIndex &index) const {
  if (!index.isValid())
    return 0;

  return Qt::ItemIsSelectable;
}




void CurrentODModel::update(ProtectedCycle &cycle) {
  if (!needUpdate)
    return;

  auto lock = cycle.getLock();

  if (!root)
    root = new CurODModelItem(nullptr, cycle, UINT8_MAX, UINT16_MAX, UINT16_MAX);

  if (node != lastUpdatedNode) {
    beginResetModel();
    delete root;
    root = new CurODModelItem(nullptr, cycle, UINT8_MAX, UINT16_MAX, UINT16_MAX);
    endResetModel();
  }

  Node *n = cycle->getNode(node);
  if (!n) {
    qDebug() << "ERROR: CurrentODModel: Node " << static_cast<int>(node) << " is not valid";
    return;
  }

  plf::colony<uint16_t> changedList = n->getOD()->getWrittenValues();
  std::vector<uint16_t> changedVec;
  changedVec.reserve(changedList.size());
  changedVec.assign(changedList.begin(), changedList.end());
  std::sort(changedVec.begin(), changedVec.end());

  lastUpdatedNode = node;
}

void CurrentODModel::updateNext() { needUpdate = true; }

void CurrentODModel::changeNode(uint8_t n) { node = n; }

void CurrentODModel::showContextMenu(const QPoint &pos) {
  (void)pos;
#if 0
  QPoint globalPos = tree->mapToGlobal(pos);

  QList<QTreeWidgetItem *> selection = tree->selectedItems();

  if (selection.size() != 1)
    return;

  QMenu myMenu;
  myMenu.addAction("Draw Plot");

  QAction *selectedItem = myMenu.exec(globalPos);
  if (selectedItem) {
    tree->selectedItems();
    bool     ok;
    uint16_t index = static_cast<uint16_t>(selection.first()->text(0).remove(0, 2).toInt(&ok, 16));
    if (!ok) {
      qDebug() << "Could not get index from text in curodmodel";
      return;
    }

    emit drawingPlot(node, index, 0);
  }
#endif
}
