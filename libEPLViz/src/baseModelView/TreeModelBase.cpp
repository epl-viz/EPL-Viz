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
 * \file TreeModelBase.cpp
 */

#include "TreeModelBase.hpp"
#include "OD.hpp"
#include <QMenu>
#include <QString>
#include <QThread>
#include <algorithm>

using namespace EPL_Viz;
using namespace EPL_DataCollect;
using namespace std;

TreeModelBase::TreeModelBase(QAbstractItemView *widget) : QAbstractItemModel(widget) { widget->setModel(this); }
TreeModelBase::~TreeModelBase() {
  if (root)
    delete root;
}


TreeModelItemBase *TreeModelBase::getItem(const QModelIndex &index) const {
  if (index.isValid()) {
    TreeModelItemBase *p = static_cast<TreeModelItemBase *>(index.internalPointer());
    if (p) {
      return p;
    } else {
      return nullptr;
    }
  }

  return root;
}


QModelIndex TreeModelBase::index(int row, int column, const QModelIndex &parent) const {
  if (parent.isValid() && parent.column() != 0)
    return QModelIndex();

  TreeModelItemBase *p = getItem(parent);
  TreeModelItemBase *c = p->child(static_cast<size_t>(row));

  if (c) {
    return createIndex(row, column, c);
  }

  return QModelIndex();
}

QModelIndex TreeModelBase::parent(const QModelIndex &index) const {
  if (!index.isValid())
    return QModelIndex();

  TreeModelItemBase *c = getItem(index);
  TreeModelItemBase *p = c->parent();

  if (p == root || p == nullptr)
    return QModelIndex();

  return createIndex(static_cast<int>(p->row()), 0, p);
}

int TreeModelBase::rowCount(const QModelIndex &parent) const {
  if (parent.column() > 0)
    return 0;

  if (!parent.isValid()) {
    if (root) {
      return root->childCount();
    } else {
      return 0;
    }
  }

  return getItem(parent)->childCount();
}

int TreeModelBase::columnCount(const QModelIndex &parent) const {
  if (parent.column() > 0)
    return 0;

  if (!parent.isValid()) {
    if (root) {
      return root->columnCount();
    } else {
      return 0;
    }
  }

  return getItem(parent)->columnCount();
}

QVariant TreeModelBase::data(const QModelIndex &index, int role) const {
  if (!index.isValid())
    return QVariant();

  TreeModelItemBase *item = getItem(index);
  return item->data(index.column(), static_cast<Qt::ItemDataRole>(role));
}

QVariant TreeModelBase::headerData(int section, Qt::Orientation orientation, int role) const {
  TreeModelRoot *rp = dynamic_cast<TreeModelRoot *>(root);

  if (!rp)
    return QVariant();

  return rp->headerData(section, orientation, role);
}

Qt::ItemFlags TreeModelBase::flags(const QModelIndex &index) const {
  if (index.isValid())
    return getItem(index)->flags();

  return 0;
}

QModelIndex TreeModelBase::indexOf(TreeModelItemBase *item, int column) const {
  if (!item)
    return QModelIndex();

  return createIndex(static_cast<int>(item->row()), column, item);
}

QModelIndex TreeModelBase::parentOf(TreeModelItemBase *item, int column) const {
  if (!item)
    return QModelIndex();

  TreeModelItemBase *p = item->parent();

  if (!p)
    return QModelIndex();

  return createIndex(static_cast<int>(item->row()), column, p);
}


void TreeModelBase::emitRowChaned(QModelIndex index) {
  QModelIndex begin = createIndex(index.row(), 0, index.internalPointer());
  QModelIndex end   = createIndex(index.row(), columnCount(index), index.internalPointer());
  emit        dataChanged(begin, end);
}

void TreeModelBase::emitRowChaned(TreeModelItemBase *item) { emitRowChaned(indexOf(item)); }
