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
 * \file CycleCommandsModel.cpp
 */

#include "CycleCommandsModel.hpp"
#include "Cycle.hpp"
#include "MainWindow.hpp"
using namespace EPL_Viz;
using namespace EPL_DataCollect;

CycleCommandsModel::CycleCommandsModel(MainWindow *mw, QTreeView *widget)
    : TreeModelBase(widget), BaseModel(mw, widget) {
  root = new TreeModelRoot({{Qt::DisplayRole,
                             {QVariant("Type"),
                              QVariant("Source"),
                              QVariant("Destination"),
                              QVariant("Relative SoC"),
                              QVariant("Relative last")}},
                            {Qt::ToolTipRole,
                             {QVariant("The packet type"),
                              QVariant("The ID of the sending node"),
                              QVariant("The ID of the reciving node"),
                              QVariant("Time passed since last Start of Cycle"),
                              QVariant("Time passed since last packet")}}});
}

CycleCommandsModel::~CycleCommandsModel() {}
void CycleCommandsModel::init() {}


void CycleCommandsModel::update(ProtectedCycle &cycle) {
  auto lock = cycle.getLock();

  if (lastCycle == cycle->getCycleNum())
    return;

  size_t numPackets = cycle->getPackets().size();

  // Delete old tree and add new
  beginResetModel();
  root->clear();

  for (size_t i = 0; i < numPackets; ++i) {
    root->push_back(std::make_unique<CyCoTreeItem>(root, cycle, i));
  }

  endResetModel();
  lastCycle = cycle->getCycleNum();
}


void CycleCommandsModel::changeNode(uint8_t) {}
