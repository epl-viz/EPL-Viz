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

CurrentODModel::CurrentODModel(MainWindow *window, QWidget *widget)
    : QAbstractItemModel(widget), BaseModel(window, widget) {
  (void)window;
  (void)widget;
  //  tree->setContextMenuPolicy(Qt::CustomContextMenu);
  //  connect(tree, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));
}

CurrentODModel::~CurrentODModel() {
  if (root)
    delete root;
}

void CurrentODModel::init() {}



CurODModelItem *CurrentODModel::getItem(const QModelIndex &index) const {
  if (index.isValid()) {
    CurODModelItem *p = static_cast<CurODModelItem *>(index.internalPointer());
    if (p) {
      return p;
    }
  }

  return root;
}


QModelIndex CurrentODModel::index(int row, int column, const QModelIndex &parent) const {
  if (parent.isValid() && parent.column() != 0)
    return QModelIndex();

  CurODModelItem *p = getItem(parent);
  CurODModelItem *c = p->child(static_cast<size_t>(row));

  if (c) {
    return createIndex(row, column, c);
  }

  return QModelIndex();
}

QModelIndex CurrentODModel::parent(const QModelIndex &index) const {
  if (!index.isValid())
    return QModelIndex();

  CurODModelItem *c = getItem(index);
  CurODModelItem *p = c->parent();

  if (p == root || p == nullptr)
    return QModelIndex();

  return createIndex(static_cast<int>(p->row()), 0, p);
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

  return getItem(parent)->columnCount();
}

QVariant CurrentODModel::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  CurODModelItem *item = getItem(index);
  return item->data(index.column(), static_cast<Qt::ItemDataRole>(role));
}

QVariant CurrentODModel::headerData(int section, Qt::Orientation orientation, int role) const {
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
    switch (section) {
      case 0: return QVariant("Index");
      case 1: return QVariant("Value");
      case 2: return QVariant("Debug");
      default: return QVariant("[ERROR: You should not see this]");
    }
  }

  return QVariant();
}

Qt::ItemFlags CurrentODModel::flags(const QModelIndex &index) const {
  if (index.isValid())
    return getItem(index)->flags();

  return 0;
}

QModelIndex CurrentODModel::indexOf(CurODModelItem *item, int column) const {
  if (!item)
    return QModelIndex();

  return createIndex(static_cast<int>(item->row()), column, item);
}

QModelIndex CurrentODModel::parentOf(CurODModelItem *item, int column) const {
  if (!item)
    return QModelIndex();

  CurODModelItem *p = item->parent();

  if (!p)
    return QModelIndex();

  return createIndex(static_cast<int>(item->row()), column, p);
}


void CurrentODModel::emitRowChaned(QModelIndex index) {
  QModelIndex begin = createIndex(index.row(), 0, index.internalPointer());
  QModelIndex end   = createIndex(index.row(), columnCount(index), index.internalPointer());
  emit        dataChanged(begin, end);
}

void CurrentODModel::emitRowChaned(CurODModelItem *item) { emitRowChaned(indexOf(item)); }


void CurrentODModel::update(ProtectedCycle &cycle) {
  auto lock = cycle.getLock();

  if (!root)
    root = new CurODModelItem(nullptr, cycle, UINT8_MAX, UINT16_MAX, UINT16_MAX);

  Node *n = cycle->getNode(node);
  if (!n) {
    return;
  }

  auto *children = root->getChildren();

  static std::vector<uint16_t> oldVec;
  static std::vector<uint16_t> chVec; // static: recycle heap memory
  static std::vector<uint16_t> diff;
  oldVec.clear();
  chVec.clear();
  diff.clear();

  for (auto &i : *root->getChildren()) {
    oldVec.emplace_back(i->getIndex());
  }

  plf::colony<uint16_t> changedList = n->getOD()->getWrittenValues();

  chVec.assign(changedList.begin(), changedList.end());
  std::sort(chVec.begin(), chVec.end());
  std::sort(oldVec.begin(), oldVec.end());
  std::set_symmetric_difference(chVec.begin(), chVec.end(), oldVec.begin(), oldVec.end(), std::back_inserter(diff));

  if (diff.empty() && node == lastUpdatedNode) {
    // No entry changes
    for (auto &i : *root->getChildren()) {
      ODEntry *entry = n->getOD()->getEntry(i->getIndex());
      if (entry->getArraySize() >= 0 && i->childCount() != entry->getArraySize()) {
        QModelIndex index = indexOf(i.get());
        beginRemoveRows(index, 0, i->childCount() - 1);
        i->clear();
        endRemoveRows();

        beginInsertRows(index, 0, entry->getArraySize() - 1);
        for (uint16_t j = 0; j < entry->getArraySize(); ++j) {
          i->push_back(std::make_unique<CurODModelItem>(i.get(), cycle, node, i->getIndex(), j));
        }
        endInsertRows();
        continue;
      }

      for (auto &j : *i->getChildren()) {
        if (j->hasChanged()) {
          emitRowChaned(j.get());
        }
      }

      if (i->hasChanged()) {
        emitRowChaned(i.get());
      }
    }
  } else {
    // Rebuild entire model (new / deleted entries are rare)
    beginResetModel();

    delete root;
    root     = new CurODModelItem(nullptr, cycle, UINT8_MAX, UINT16_MAX, UINT16_MAX);
    children = root->getChildren();

    for (auto i : chVec) {
      ODEntry *entry = n->getOD()->getEntry(i);

      if (!entry) {
        qDebug() << "ERROR entry does not exist! " << i;
        continue;
      }

      auto            uPtr = std::make_unique<CurODModelItem>(root, cycle, node, i);
      CurODModelItem *item = uPtr.get();
      root->push_back(std::move(uPtr));

      for (uint16_t j = 0; j < entry->getArraySize() && j <= 0xFF; ++j) {
        item->push_back(std::make_unique<CurODModelItem>(item, cycle, node, i, j));
      }
    }

    endResetModel();
  }

  lastUpdatedNode = node;
}

void CurrentODModel::updateNext() {}

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
