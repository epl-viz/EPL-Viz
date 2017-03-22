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

#include "SettingsPlotItem.hpp"

using namespace EPL_Viz;

SettingsPlotItem::SettingsPlotItem(TreeModelItemBase *parent, PlotCreator::PlotCreatorData d)
    : TreeModelItemBase(parent), pltData(d) {}

SettingsPlotItem::~SettingsPlotItem() {}

Qt::ItemFlags SettingsPlotItem::flags() { return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemNeverHasChildren; }
bool          SettingsPlotItem::hasChanged() { return false; }

QVariant SettingsPlotItem::dataDisplay(int column) {
  switch (column) {
    case 0: // Plot Type
      if (pltData.defaultODPlot)
        return "OD Plot";
      else
        return "CS Plot";

    case 1: // Add to
      if (pltData.addToPlot && pltData.addToTimeLine)
        return "TL / PLOT";

      if (pltData.addToPlot)
        return "Plot";

      if (pltData.addToTimeLine)
        return "Timeline";

      return "Nothing";

    case 2: // Index
      if (pltData.defaultODPlot)
        return QString().number(static_cast<int>(pltData.index), 16).prepend("0x");

      return pltData.csName.c_str();

    case 3: // Subindex
      if (pltData.defaultODPlot)
        return QString().number(static_cast<int>(pltData.subIndex), 16).prepend("0x");

      return QVariant();

    case 4: // Node
      if (pltData.defaultODPlot)
        return QString().number(static_cast<int>(pltData.node), 16).prepend("0x");

      return QVariant();

    default: return QVariant();
  }
}


QVariant SettingsPlotItem::dataTooltip(int column) {
  switch (column) {
    case 0: // Plot Type
      if (pltData.defaultODPlot)
        return "A normal Object Dictionary plot";
      else
        return "Plot over a custom CycleStorage (independent of node and OD)";

    case 1: // Add to
      if (pltData.addToPlot && pltData.addToTimeLine)
        return "This Plot will be added to both the Timeline and the main plot widget";

      if (pltData.addToPlot)
        return "This Plot will be added to the main plot widget";

      if (pltData.addToTimeLine)
        return "This Plot will be added to the Timeline";

      return "This Plot will be added to nothing, the data will still be accumulated";

    default: return QVariant();
  }
}

QColor SettingsPlotItem::dataBackground() { return pltData.color; }

QColor SettingsPlotItem::dataForground() {
  QColor c = dataBackground();
  if (!c.isValid())
    return QColor();

  if (c.lightness() >= 125) {
    return QColor("#000000");
  }

  return QColor("#ffffff");
}


QVariant SettingsPlotItem::data(int column, Qt::ItemDataRole role) {
  QColor col;
  switch (role) {
    case Qt::DisplayRole: return dataDisplay(column);
    case Qt::ToolTipRole: return dataTooltip(column);
    case Qt::BackgroundRole: col = dataBackground(); break;
    case Qt::ForegroundRole: col = dataForground(); break;
    default: return QVariant();
  }

  if (!col.isValid())
    return QVariant();

  return QBrush(col);
}
