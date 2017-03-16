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

CurrentODModel::CurrentODModel(MainWindow *window, QTreeView *widget)
    : TreeModelBase(widget), BaseModel(window, widget) {
  root = new TreeModelRoot({{Qt::DisplayRole, {QVariant("Index"), QVariant("Value")}}});
  view = widget;
}

CurrentODModel::~CurrentODModel() {}

void CurrentODModel::init() {}

void CurrentODModel::update(ProtectedCycle &cycle) {
  auto l    = getLock();
  auto lock = cycle.getLock();

  Node *n = cycle->getNode(node);
  if (!n) {
    return;
  }

  static std::vector<uint16_t> oldVec;
  static std::vector<uint16_t> chVec; // static: recycle heap memory
  static std::vector<uint16_t> diff;
  oldVec.clear();
  chVec.clear();
  diff.clear();

  for (auto &i : *root->getChildren()) {
    oldVec.emplace_back(dynamic_cast<CurODModelItem *>(i)->getIndex());
  }

  plf::colony<uint16_t> changedList = n->getOD()->getWrittenValues();

  chVec.assign(changedList.begin(), changedList.end());
  std::sort(chVec.begin(), chVec.end());
  std::sort(oldVec.begin(), oldVec.end());
  std::set_symmetric_difference(chVec.begin(), chVec.end(), oldVec.begin(), oldVec.end(), std::back_inserter(diff));

  std::vector<uint32_t> toHighLight;

  auto events = cycle->getActiveEvents();
  for (auto i : events) {
    if (i->getType() == EvType::VIEW_EV_HIGHLIGHT_OD_ENTRY) {
      for (auto j : i->getAffectedIndices()) {
        toHighLight.push_back(j.first);
      }
    }
  }

  if (diff.empty() && node == lastUpdatedNode) {
    // No entry changes
    for (auto &i : *root->getChildren()) {
      ODEntry *entry = n->getOD()->getEntry(dynamic_cast<CurODModelItem *>(i)->getIndex());
      if (entry->getArraySize() >= 0 && i->childCount() != entry->getArraySize()) {
        QModelIndex index = indexOf(i);
        beginRemoveRows(index, 0, i->childCount() - 1);
        i->clear();
        endRemoveRows();

        beginInsertRows(index, 0, entry->getArraySize() - 1);
        for (uint16_t j = 0; j < entry->getArraySize(); ++j) {
          i->push_back(new CurODModelItem(i, cycle, node, dynamic_cast<CurODModelItem *>(i)->getIndex(), j));
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

      dynamic_cast<CurODModelItem *>(i)->resetColor();
      if (i->hasChanged()) {
        emitRowChaned(i);
      }

      if (std::find(toHighLight.begin(), toHighLight.end(), dynamic_cast<CurODModelItem *>(i)->getIndex()) !=
          toHighLight.end()) {
        dynamic_cast<CurODModelItem *>(i)->setColor(QColor(0x7c, 0xd1, 0xd9));
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

    endResetModel();
  }

  lastUpdatedNode = node;
}

void CurrentODModel::updateNext() {}

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
