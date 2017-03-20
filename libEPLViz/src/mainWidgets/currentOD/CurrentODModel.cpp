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
#include "CurODCycleStorage.hpp"
#include "OD.hpp"
#include <QMenu>
#include <QString>
#include <QThread>
#include <algorithm>

using namespace EPL_Viz;
using namespace EPL_DataCollect;
using namespace std;

CurrentODModel::CurrentODModel(MainWindow *window, QTreeView *widget)
    : TreeModelBase(widget), BaseModel(window, widget) {
  root   = new TreeModelRoot({{Qt::DisplayRole, {QVariant("Index"), QVariant("Value"), QVariant("Name")}}});
  filter = new ODFilter(this, window);
  filter->setSourceModel(this);
  widget->setModel(filter);
  view = widget;
}

CurrentODModel::~CurrentODModel() {}

void CurrentODModel::init() {
  node            = 1;
  lastUpdatedNode = node;

  beginResetModel();
  root->clear();
  endResetModel();
}

void CurrentODModel::update(ProtectedCycle &cycle) {
  auto l    = getLock();
  auto lock = cycle.getLock();

  Node *n = cycle->getNode(node);
  if (!n) {
    return;
  }

  bool hasFilterChanged = filter->updateFilter();

  static std::vector<uint16_t>    oldVec;
  static std::vector<std::string> oldVecCS;
  static std::vector<uint16_t>    chVec; // static: recycle heap memory
  static std::vector<std::string> chVecCS;
  static std::vector<uint16_t>    diff;
  static std::vector<std::string> diffCS;
  oldVec.clear();
  oldVecCS.clear();
  chVec.clear();
  chVecCS.clear();
  diff.clear();
  diffCS.clear();

  for (auto &i : *root->getChildren()) {
    CurODModelItem *   od = dynamic_cast<CurODModelItem *>(i);
    CurODCycleStorage *cs = dynamic_cast<CurODCycleStorage *>(i);

    if (od) {
      oldVec.emplace_back(od->getIndex());
    } else if (cs) {
      oldVecCS.emplace_back(cs->getIndex());
    }
  }

  for (auto &i : *cycle->getAllCycleStorage()) {
    chVecCS.emplace_back(i.first);
  }

  plf::colony<uint16_t> changedList = n->getOD()->getWrittenValues();

  chVec.assign(changedList.begin(), changedList.end());
  std::sort(chVec.begin(), chVec.end());
  std::sort(oldVec.begin(), oldVec.end());
  set_symmetric_difference(chVec.begin(), chVec.end(), oldVec.begin(), oldVec.end(), back_inserter(diff));
  set_symmetric_difference(chVecCS.begin(), chVecCS.end(), oldVecCS.begin(), oldVecCS.end(), back_inserter(diffCS));


  std::vector<uint32_t> toHighLight;

  auto events = cycle->getActiveEvents();
  for (auto i : events) {
    if (i->getType() == EvType::VIEW_EV_HIGHLIGHT_OD_ENTRY) {
      for (auto j : i->getAffectedIndices()) {
        toHighLight.push_back(static_cast<uint32_t>(j.first));
      }
    }
  }

  if (diff.empty() && diffCS.empty() && node == lastUpdatedNode && !hasFilterChanged) {
    // No entry changes
    for (auto &i : *root->getChildren()) {
      CurODModelItem *od = dynamic_cast<CurODModelItem *>(i);
      if (!od) {
        CurODCycleStorage *cs = dynamic_cast<CurODCycleStorage *>(i);

        if (!cs)
          continue;

        if (cs->hasChanged())
          emitRowChaned(i);

        continue;
      }

      ODEntry *entry = n->getOD()->getEntry(od->getIndex());
      if (entry->getArraySize() >= 0 && i->childCount() != entry->getArraySize()) {
        QModelIndex index = indexOf(i);
        beginRemoveRows(index, 0, i->childCount() - 1);
        i->clear();
        endRemoveRows();

        beginInsertRows(index, 0, entry->getArraySize() - 1);
        for (uint16_t j = 0; j < entry->getArraySize(); ++j) {
          i->push_back(new CurODModelItem(i, cycle, node, od->getIndex(), j));
        }
        endInsertRows();
        continue;
      }

      for (auto &j : *i->getChildren()) {
        dynamic_cast<CurODModelItem *>(j)->resetColor();
        if (j->hasChanged()) {
          emitRowChaned(j);
        }
      }

      od->resetColor();
      if (i->hasChanged()) {
        emitRowChaned(i);
      }

      if (std::find(toHighLight.begin(), toHighLight.end(), od->getIndex()) != toHighLight.end()) {
        od->setColor(QColor(0x7c, 0xd1, 0xd9));
      }
    }
  } else {
    // Rebuild entire model (new / deleted entries are rare)
    beginResetModel();

    root->clear();

    for (auto i : chVec) {
      ODEntry *entry = n->getOD()->getEntry(i);

      if (!entry) {
        qDebug() << "ERROR entry does not exist! " << i;
        continue;
      }

      root->push_back(new CurODModelItem(root, cycle, node, i));
      auto *item = root->back();

      for (uint16_t j = 0; j < entry->getArraySize() && j <= 0xFF; ++j) {
        item->push_back(new CurODModelItem(item, cycle, node, i, j));
      }

      if (std::find(toHighLight.begin(), toHighLight.end(), dynamic_cast<CurODModelItem *>(item)->getIndex()) !=
          toHighLight.end()) {
        dynamic_cast<CurODModelItem *>(item)->setColor(QColor(0x7c, 0xd1, 0xd9));
      }
    }

    for (auto i : chVecCS) {
      CycleStorageBase *cs = cycle->getCycleStorage(i);

      if (!cs) {
        qDebug() << "ERROR Cycle storage " << i.c_str() << " does not exist!";
        continue;
      }

      root->push_back(new CurODCycleStorage(root, cycle, i));
    }

    endResetModel();
  }

  lastUpdatedNode = node;
}

void CurrentODModel::changeNode(uint8_t n) { node = n; }

void CurrentODModel::showContextMenu(const QPoint &pos) {
  QMenu myMenu;
  myMenu.addAction("Draw Plot");

  QModelIndex index = view->indexAt(pos);
  if (index.isValid()) {
    QAction *selectedAction = myMenu.exec(view->mapToGlobal(pos));
    if (selectedAction) {
      // QModelIndex indexCol = createIndex(index.row(), 0, index.internalPointer());
      // QVariant dataCol = data(indexCol, Qt::DisplayRole);
      CurODModelItem *item = static_cast<CurODModelItem *>(index.internalPointer());
      emit            drawingPlot(node, item->getIndex(), item->getSubIndex());
      // TODO get real data
      /*
      ->selectedItems();
      bool     ok;
      uint16_t i = static_cast<uint16_t>(tion.first()->text(0).remove(0, 2).toInt(&ok, 16));
      if (!ok) {
        qDebug() << "Could not get index from text in curodmodel";
        return;
      }

      emit drawingPlot(node, index.data, 0);
      */
    }
  }
}
