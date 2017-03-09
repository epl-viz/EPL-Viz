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

CurODModelItem::~CurODModelItem() {}


CurODModelItem *CurODModelItem::parent() { return p; }

CurODModelItem *CurODModelItem::child(size_t row) {
  if (row > childItems.size())
    return nullptr;

  return childItems[row].get();
}

void CurODModelItem::push_back(std::unique_ptr<CurODModelItem> item) {
  childIndexMap[item.get()] = childItems.size();
  childItems.emplace_back(std::move(item));
}

void CurODModelItem::clear() {
  childItems.clear();
  childIndexMap.clear();
}

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

      if (subIndex == UINT16_MAX) // This is a root OD item
        return QVariant(QString().number(static_cast<int>(index), 16).prepend("0x"));
      else // This is a subIndex item
        return QVariant(QString().number(static_cast<int>(subIndex), 16).prepend("0x"));

    case 1: // VALUE

      if (subIndex == UINT16_MAX) { // This is a root OD item
        if (entry->getArraySize() >= 0) {
          return QVariant();
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

CurODModelItem::LIST *CurODModelItem::getChildren() { return &childItems; }
int                   CurODModelItem::childCount() const { return static_cast<int>(childItems.size()); }
int                   CurODModelItem::columnCount() const { return 2; }


size_t CurODModelItem::indexOf(CurODModelItem const *item) const { return childIndexMap.at(item); }

size_t CurODModelItem::row() const {
  if (!p)
    return 0;

  return p->indexOf(this);
}

Qt::ItemFlags CurODModelItem::flags() { return Qt::ItemIsEnabled; }

bool CurODModelItem::hasChanged() { return true; }

uint16_t CurODModelItem::getIndex() const { return index; }
