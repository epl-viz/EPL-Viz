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
 * \file CurODModelItem.cpp
 */
#include "CurODModelItem.hpp"
using namespace EPL_Viz;
using namespace EPL_DataCollect;

CurODModelItem::CurODModelItem(
      CurODModelItem *parent, ProtectedCycle &cycle, uint8_t cNode, uint16_t odIndex, uint16_t odSubIndex)
    : p(parent), c(cycle), node(cNode), index(odIndex), subIndex(odSubIndex) {}

CurODModelItem::~CurODModelItem() { qDeleteAll(childItems); }


void CurODModelItem::setChildItems(QList<CurODModelItem *> items) {
  qDeleteAll(childItems);
  childItems = items;
}


CurODModelItem *CurODModelItem::parent() { return p; }

CurODModelItem *CurODModelItem::child(int row) { return childItems.value(row, nullptr); }

QVariant CurODModelItem::data(int column, Qt::ItemDataRole role) {
  if (role != Qt::DisplayRole)
    return QVariant();

  auto  lock = c.getLock();
  Node *n    = c->getNode(node);
  if (!n)
    return QVariant("[INVALID NODE]");

  ODEntry *entry = n->getOD()->getEntry(index);

  if (!entry) {
    QString error = "[INVALID OD INDEX ";
    error += std::to_string(index).c_str();
    error += "]";
    return QVariant(error);
  }

  switch (column) {
    case 0: // INDEX

      if (subIndex > UINT8_MAX) // This is a root OD item
        return QVariant(static_cast<int>(index));
      else // This is a subIndex item
        return QVariant(static_cast<int>(subIndex));

    case 1: // VALUE

      if (subIndex > UINT8_MAX) { // This is a root OD item
        if (entry->getArraySize() < 0) {
          return QVariant("<ROOT INDEX>");
        }
        return QVariant(entry->toString().c_str());
      } else { // This is a subIndex item
        if (entry->getArraySize() < 0) {
          return QVariant("[ERROR: THIS ENTRY HAS NO SUBINDECES]");
        }

        return QVariant(entry->toString(static_cast<uint8_t>(subIndex)).c_str());
      }

    default: return QVariant("[INVALID COLUMN]");
  }
}

QList<CurODModelItem *> *CurODModelItem::getChildren() { return &childItems; }
int                      CurODModelItem::childCount() const { return childItems.size(); }
int                      CurODModelItem::columnCount() const { return 2; }
int                      CurODModelItem::row() const {
  if (p)
    return p->childItems.indexOf(const_cast<CurODModelItem *>(this));

  return 0;
}