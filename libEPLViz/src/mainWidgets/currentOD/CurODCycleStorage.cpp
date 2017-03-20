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
#include "CurODCycleStorage.hpp"
using namespace EPL_Viz;
using namespace EPL_DataCollect;

CurODCycleStorage::CurODCycleStorage(
      TreeModelItemBase *parent, ProtectedCycle &cycle, std::string csName, bool isColor, QColor col)
    : TreeModelItemBase(parent), c(cycle), index(csName), hasColor(isColor), color(col) {}

CurODCycleStorage::~CurODCycleStorage() {}


Qt::ItemFlags CurODCycleStorage::flags() { return Qt::ItemIsEnabled; }
bool          CurODCycleStorage::hasChanged() { return true; }
std::string   CurODCycleStorage::getIndex() const { return index; }
void          CurODCycleStorage::resetColor() { hasColor = false; }
void CurODCycleStorage::setColor(QColor col) {
  color    = col;
  hasColor = true;
}

QVariant CurODCycleStorage::dataDisplay(int column) {
  CycleStorageBase *cs = c->getCycleStorage(index);
  if (!cs)
    return QVariant("[INVALID NODE]");

  auto str = cs->getStringRepresentation();

  switch (column) {
    case 0: // INDEX
      return QVariant(index.c_str());

    case 1: // VALUE
      if (cs->isNumericValue())
        return QVariant(cs->getNumericValue());

      return QVariant(str.empty() ? "<NO VALUE>" : str.c_str());

    case 2: // NAME
      return QVariant();

    default: return QVariant("[INVALID COLUMN]");
  }
}

QVariant CurODCycleStorage::dataTooltip(int column) {
  switch (column) {
    case 0: return QVariant("The OD index");
    case 1: return QVariant("The Value");
    default: return QVariant();
  }
}

QVariant CurODCycleStorage::dataBackground(int) {
  if (hasColor)
    return QBrush(color);

  return QVariant();
}

QVariant CurODCycleStorage::data(int column, Qt::ItemDataRole role) {
  auto lock = c.getLock();

  switch (role) {
    case Qt::DisplayRole: return dataDisplay(column);
    case Qt::ToolTipRole: return dataTooltip(column);
    case Qt::BackgroundRole: return dataBackground(column);
    default: return QVariant();
  }
}
