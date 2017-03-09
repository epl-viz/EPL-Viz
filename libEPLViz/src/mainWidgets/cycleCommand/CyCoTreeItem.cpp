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
 * \file CyCoTreeItem.cpp
 */
#include "CyCoTreeItem.hpp"
using namespace EPL_Viz;

CyCoTreeItem::CyCoTreeItem(const QVector<QVariant> &data, CyCoTreeItem *parent) {
  parentItem = parent;
  itemData   = data;
}

CyCoTreeItem::~CyCoTreeItem() { qDeleteAll(childItems); }

CyCoTreeItem *CyCoTreeItem::parent() { return parentItem; }

CyCoTreeItem *CyCoTreeItem::child(int number) { return childItems.value(number); }

int CyCoTreeItem::childCount() const { return childItems.count(); }

int CyCoTreeItem::childNumber() const {
  if (parentItem)
    return parentItem->childItems.indexOf(const_cast<CyCoTreeItem *>(this));
  return 0;
}
int CyCoTreeItem::columnCount() const { return itemData.count(); }

QVariant CyCoTreeItem::data(int column) const { return itemData.value(column); }

bool CyCoTreeItem::setData(int column, const QVariant &value) {
  if (column < 0 || column >= itemData.size())
    return false;

  itemData[column] = value;
  return true;
}

bool CyCoTreeItem::insertChildren(int position, int count) {
  if (position < 0 || position > childItems.size())
    return false;

  for (int row = 0; row < count; ++row) {
    QVector<QVariant> data(columnCount());
    CyCoTreeItem *    item = new CyCoTreeItem(data, this);
    childItems.insert(position, item);
  }

  return true;
}

bool CyCoTreeItem::removeChildren(int position, int count) {
  if (position < 0 || position + count > childItems.size())
    return false;

  for (int row = 0; row < count; ++row)
    delete childItems.takeAt(position);

  return true;
}
