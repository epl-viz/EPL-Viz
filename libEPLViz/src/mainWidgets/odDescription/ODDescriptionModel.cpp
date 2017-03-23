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
 * \file ODDescriptionModel.cpp
 */
#include "ODDescriptionModel.hpp"
#include "OD.hpp"
#include "ODDescriptionItem.hpp"
#include <QMenu>
#include <QString>
#include <algorithm>

using namespace EPL_Viz;
using namespace EPL_DataCollect;
using namespace std;

ODDescriptionModel::ODDescriptionModel(MainWindow *window, QTreeView *treeWidget)
    : TreeModelBase(treeWidget), BaseModel(window, treeWidget) {
  root = new TreeModelRoot(
        {{Qt::DisplayRole,
          {QVariant("Index"), QVariant("Name"), QVariant("Type"), QVariant("Data Type"), QVariant("Default Value")}},
         {Qt::ToolTipRole,
          {QVariant("The HEX index of the OD entry"),
           QVariant("The type of the OD entry"),
           QVariant("The data type of the OD entry"),
           QVariant("The default value specified in the XDD (0 if not specified)")}}});

  filter = new ODFilterDesc(treeWidget, window);
  filter->setSourceModel(this);
  treeWidget->setModel(filter);
}

void ODDescriptionModel::init() {
  node = UINT8_MAX;

  beginResetModel();
  root->clear();
  endResetModel();
}

void ODDescriptionModel::update() {
  ProtectedCycle &cycle    = BaseModel::getCurrentCycle();
  auto            l        = getLock();
  int             children = root->childCount();
  auto            lock     = cycle.getLock();
  Node *          n        = cycle->getNode(node);

  // Check if no node is selected
  if (node == UINT8_MAX || !n) {
    // Clear the widget if necessary
    if (children > 0)
      hasChanged = true;

    return;
  }

  bool hasFilterChanged = filter->updateFilter();

  static std::vector<uint16_t> oldVec;
  static std::vector<uint16_t> chVec; // static: recycle heap memory
  static std::vector<uint16_t> diff;
  oldVec.clear();
  chVec.clear();
  diff.clear();

  for (auto &i : *root->getChildren()) {
    oldVec.emplace_back(dynamic_cast<ODDescriptionItem *>(i)->getIndex());
  }

  plf::colony<uint16_t> changedList = n->getOD()->getODDesc()->getEntriesList();

  chVec.assign(changedList.begin(), changedList.end());
  std::sort(chVec.begin(), chVec.end());
  std::sort(oldVec.begin(), oldVec.end());
  std::set_symmetric_difference(chVec.begin(), chVec.end(), oldVec.begin(), oldVec.end(), std::back_inserter(diff));

  if (!diff.empty() || hasFilterChanged)
    hasChanged = true;
}

void ODDescriptionModel::updateWidget() {
  if (!hasChanged)
    return;

  ProtectedCycle &cycle = BaseModel::getCurrentCycle();
  auto            l     = getLock();
  auto            lock  = cycle.getLock();
  Node *          n     = cycle->getNode(node);

  beginResetModel();
  root->clear();

  if (node == UINT8_MAX || !n)
    return;

  static std::vector<uint16_t> chVec; // static: recycle heap memory
  chVec.clear();

  plf::colony<uint16_t> changedList = n->getOD()->getODDesc()->getEntriesList();

  chVec.assign(changedList.begin(), changedList.end());
  std::sort(chVec.begin(), chVec.end());

  for (auto i : chVec) {
    ODEntryDescription *entry = n->getOD()->getODDesc()->getEntry(i);

    if (!entry) {
      qDebug() << "ERROR entry does not exist! " << i;
      continue;
    }

    root->push_back(new ODDescriptionItem(root, cycle, node, i));
    auto *item = root->back();

    for (uint16_t j = 0; j < entry->subEntries.size(); ++j) {
      item->push_back(new ODDescriptionItem(item, cycle, node, i, j));
    }
  }

  endResetModel();
  hasChanged = false;
}


void ODDescriptionModel::selectNode(uint8_t n) {
  if (node != n) {
    node = n;
    update();
  }
}
