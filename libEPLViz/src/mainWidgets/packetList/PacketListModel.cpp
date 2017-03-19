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
 * \file ODDescriptionModel.cpp
 */
#include "PacketListModel.hpp"
#include "MainWindow.hpp"
#include <QString>

using namespace EPL_Viz;
using namespace EPL_DataCollect;
using namespace std;

PacketListModel::PacketListModel(MainWindow *window, QTreeView *treeWidget)
    : TreeModelBase(treeWidget), BaseModel(window, treeWidget), mw(window) {
  root = new TreeModelRoot({{Qt::DisplayRole,
                             {QVariant("Number"),
                              QVariant("Cycle"),
                              QVariant("Packet Type"),
                              QVariant("Source"),
                              QVariant("Destination"),
                              QVariant("NMTState"),
                              QVariant("Service ID"),
                              QVariant("Command")}},
                            {Qt::ToolTipRole,
                             {QVariant("The Packet Frame Number"),
                              QVariant("The Cycle number"),
                              QVariant("The Type of the packet"),
                              QVariant("The source CN ID of the packet"),
                              QVariant("The destination CN ID of the packet"),
                              QVariant("The send NMT Status (if the packet type supports it)"),
                              QVariant("The send Service ID (if the packet type supports it)"),
                              QVariant("The send Command (if the packet type supports it)")}}});

  root->setNoDelete(true);
}

void PacketListModel::init() {
  currentPacketListSize = 0;

  beginResetModel();
  root->clear();
  itemList.clear();
  endResetModel();
}

void PacketListModel::update(ProtectedCycle &cycle) {
  auto             l  = getLock();
  CaptureInstance *ci = mw->getCaptureInstance();

  if (!ci)
    return;

  InputHandler *ih          = ci->getInputHandler();
  auto          listWrapper = ih->getPacketsMetadata();
  auto &        list        = *listWrapper;
  size_t        s           = list.size();

  if (s > currentPacketListSize) {
    beginInsertRows(QModelIndex(), static_cast<int>(currentPacketListSize), static_cast<int>(s - 1));
    for (size_t i = currentPacketListSize; i < s; ++i) {
      auto it = itemList.emplace(root, mw, list[i], i);
      root->push_back(&(*it));
    }
    endInsertRows();
    currentPacketListSize = s;
  } else if (s < currentPacketListSize) {
    // Clear the list
    currentPacketListSize = 0;
    beginResetModel();
    root->clear();
    endResetModel();
    update(cycle);
    return;
  }
}
