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

void CurrentODModel::init() {}



QModelIndex CurrentODModel::index(int row, int column, const QModelIndex &parent) const {
  (void)row;
  (void)column;
  (void)parent;
  return QModelIndex();
}

QModelIndex CurrentODModel::parent(const QModelIndex &index) const {
  (void)index;
  return QModelIndex();
}

int CurrentODModel::rowCount(const QModelIndex &parent) const {
  (void)parent;
  return 0;
}

int CurrentODModel::columnCount(const QModelIndex &parent) const {
  (void)parent;
  return 0;
}

QVariant CurrentODModel::data(const QModelIndex &index, int role) const {
  (void)index;
  (void)role;
  return QVariant("TODO");
}

QVariant CurrentODModel::headerData(int section, Qt::Orientation orientation, int role) const {
  (void)section;
  (void)orientation;
  (void)role;
  return QVariant("TODO");
}



void CurrentODModel::update(ProtectedCycle &cycle) {
  if (!needUpdate)
    return;

  auto lock = cycle.getLock();

  emit(updateExternal(*cycle, node));
  qDebug() << "Block ended";
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
