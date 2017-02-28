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
#include "ODEntryDescription.hpp"
using namespace EPL_Viz;
using namespace EPL_DataCollect;

CurrentODModel::CurrentODModel(QMainWindow *window) : BaseModel() {
  QTableView  *view = window->findChild<QTableView *>("curNodeODView");
  view->setModel(this);
  view->verticalHeader()->hide();
}

void CurrentODModel::init() {}

int CurrentODModel::rowCount(const QModelIndex &parent) const {
  // TODO
  (void)parent;
  return 1;
}

int CurrentODModel::columnCount(const QModelIndex &parent) const {
  (void)parent;
  return 3;
}

QVariant CurrentODModel::data(const QModelIndex &index, int role) const {
  // TODO
  if (role == Qt::DisplayRole) {
    return QString("Row%1, Column%2").arg(index.row() + 1).arg(index.column() + 1);
  }
  return QVariant();
}

QVariant CurrentODModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal) {
            switch (section)
            {
            // TODO ODEntryDescription
            case 0:
                return QString("Index");
            case 1:
                return QString("Type");
            case 2:
                return QString("Value");
            }
        }
    }
    return QVariant();
}

void CurrentODModel::update(EPL_DataCollect::Cycle *cycle) {
  ODDescription *description = cycle->getNode(node)->getODDesc();
  (void)description;
}
