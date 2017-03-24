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
#include "MainWindow.hpp"
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
  lastUpdatedNode = node;

  beginResetModel();
  root->clear();
  endResetModel();
}

void CurrentODModel::update() {
  ProtectedCycle &cycle    = BaseModel::getCurrentCycle();
  auto            l        = getLock();
  int             children = root->childCount();
  auto            lock     = cycle.getLock();
  Node *          n        = cycle->getNode(node);

  if (!n || node == UINT8_MAX) {
    // Clear the widget if necessary
    if (children > 0) {
      hasChanged    = true;
      completeReset = true;
    }
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

  if (diff.empty() && diffCS.empty() && node == lastUpdatedNode && !hasFilterChanged) {
    hasChanged = true;
  } else {
    hasChanged    = true;
    completeReset = true;
  }

  lastUpdatedNode = node;
}

void CurrentODModel::updateWidget() {
  if (!hasChanged)
    return;

  ProtectedCycle &cycle    = BaseModel::getCurrentCycle();
  auto            l        = getLock();
  int             children = root->childCount();

  auto  lock = cycle.getLock();
  Node *n    = cycle->getNode(node);

  // Check if no node is selected
  if (node == UINT8_MAX || !n) {
    // Clear the widget if necessary
    if (children > 0) {
      beginResetModel();
      root->clear();
      endResetModel();
    }
    return;
  }

  std::vector<uint32_t> toHighLight;

  auto events = cycle->getActiveEvents();
  for (auto i : events) {
    if (i->getType() == EvType::VIEW_EV_HIGHLIGHT_OD_ENTRY) {
      for (auto j : i->getAffectedIndices()) {
        toHighLight.push_back(static_cast<uint32_t>(j.first));
      }
    }
  }

  if (completeReset) {
    beginResetModel();
    root->clear();

    static std::vector<uint16_t>    chVec; // static: recycle heap memory
    static std::vector<std::string> chVecCS;
    chVec.clear();
    chVecCS.clear();

    for (auto &i : *cycle->getAllCycleStorage()) {
      chVecCS.emplace_back(i.first);
    }

    plf::colony<uint16_t> changedList = n->getOD()->getWrittenValues();

    chVec.assign(changedList.begin(), changedList.end());
    std::sort(chVec.begin(), chVec.end());

    if (!n || node == UINT8_MAX)
      return;

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
        dynamic_cast<CurODModelItem *>(item)->setColor(getMainWindow()->getSettingsWin()->getConfig().odHighlight);
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
  } else {
    // No entry changes
    for (auto &i : *root->getChildren()) {
      CurODModelItem *od = dynamic_cast<CurODModelItem *>(i);
      if (!od)
        continue;


      od->resetColor();
      if (std::find(toHighLight.begin(), toHighLight.end(), od->getIndex()) != toHighLight.end()) {
        od->setColor(getMainWindow()->getSettingsWin()->getConfig().odHighlight);
      }
    }

    emit dataChanged(index(0, 0, QModelIndex()),
                     index(rowCount(QModelIndex()) - 1, 2, QModelIndex()),
                     {Qt::DisplayRole, Qt::BackgroundRole});
  }

  completeReset = false;
  hasChanged    = false;
}

void CurrentODModel::selectNode(uint8_t n) {
  if (node != n) {
    node = n;
    forceNextUpdate();
  }
}

void CurrentODModel::showContextMenu(const QPoint &pos) {
  QMenu myMenu;
  myMenu.addAction("Draw Plot");

  QModelIndex index = view->indexAt(pos);
  if (index.isValid()) {
    QAction *selectedAction = myMenu.exec(view->mapToGlobal(pos));
    if (selectedAction) {
      CurODModelItem *item = static_cast<CurODModelItem *>(index.internalPointer());
      emit            drawingPlot(node, item->getIndex(), item->getSubIndex(), "", QColor(0, 0, 0));
    }
  }
}
