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

#include "ODDescriptionItem.hpp"
#include "EPLEnum2Str.hpp"

using namespace EPL_Viz;
using namespace EPL_DataCollect;

ODDescriptionItem::ODDescriptionItem(
      TreeModelItemBase *parent, ProtectedCycle &cycle, uint8_t cNode, uint16_t odIndex, uint16_t odSubIndex)
    : TreeModelItemBase(parent), c(cycle), node(cNode), index(odIndex), subIndex(odSubIndex) {}

ODDescriptionItem::~ODDescriptionItem() {}

Qt::ItemFlags ODDescriptionItem::flags() { return Qt::ItemIsEnabled; }
bool          ODDescriptionItem::hasChanged() { return false; }
uint16_t      ODDescriptionItem::getIndex() const { return index; }

QVariant ODDescriptionItem::data(int column, Qt::ItemDataRole role) {
  if (role != Qt::DisplayRole)
    return QVariant();

  auto  lock = c.getLock();
  Node *n    = c->getNode(node);
  if (!n)
    return QVariant("[INVALID NODE]");

  ODEntryDescription *entry;
  ODEntryDescription *temp = n->getOD()->getODDesc()->getEntry(index);

  if (!temp)
    return QVariant("[INVALID OD INDEX]");

  if (subIndex == UINT16_MAX)
    entry = temp;
  else
    entry = &temp->subEntries[subIndex];

  if (!entry)
    return QVariant("[INVALID OD INDEX]");

  switch (column) {
    case 0: // INDEX

      if (subIndex == UINT16_MAX) // This is a root OD item
        return QVariant(QString().number(static_cast<int>(index), 16).prepend("0x"));
      else // This is a subIndex item
        return QVariant(QString().number(static_cast<int>(subIndex), 16).prepend("0x"));

    case 1: // NAME
      return QVariant(entry->name.c_str());

    case 2: // TYPE
      return QVariant(EPLEnum2Str::toStr(entry->type).c_str());

    case 3: // DATA Type
      return QVariant(EPLEnum2Str::toStr(entry->dataType).c_str());

    case 4: // DEFAULT Value
      return QVariant(entry->defaultValue->toString().c_str());

    default: return QVariant("[INVALID COLUMN]");
  }
}
