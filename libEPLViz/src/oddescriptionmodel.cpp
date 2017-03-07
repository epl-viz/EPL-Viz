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
 * \file oddescriptionmodel.cpp
 */
#include "oddescriptionmodel.hpp"
#include "OD.hpp"
#include <QMenu>
#include <QString>
#include <QThread>
#include <algorithm>
using namespace EPL_Viz;
using namespace EPL_DataCollect;
using namespace std;

ODDescpriptonModel::ODDescpriptonModel(QMainWindow *window) : BaseModel() {
  tree = window->findChild<QTreeWidget *>("odDescriptionWidget");
  tree->setContextMenuPolicy(Qt::CustomContextMenu);
  tree->setSortingEnabled(true);
  connect(tree, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(showContextMenu(const QPoint &)));
  needUpdate = true;
}

void ODDescpriptonModel::init() {}



void ODDescpriptonModel::update(EPL_DataCollect::Cycle *cycle) {
  if (!needUpdate)
    return;

  emit(updateExternal(cycle, node));
  qDebug() << "Block ended";

  /*
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
  }*/
}

void ODDescpriptonModel::updateNext() { needUpdate = true; }

void ODDescpriptonModel::changeNode(uint8_t n) { node = n; }

void ODDescpriptonModel::showContextMenu(const QPoint &pos) {
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
    uint16_t index = selection.first()->text(0).remove(0, 2).toInt(&ok, 16);
    if (!ok) {
      qDebug() << "Could not get index from text in curodmodel";
      return;
    }

    emit drawingPlot(node, index, 0);
  }
}
