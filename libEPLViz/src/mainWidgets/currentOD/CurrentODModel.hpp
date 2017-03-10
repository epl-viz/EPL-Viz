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
 * \file CurrentODModel.hpp
 */
#pragma once

#include "BaseModel.hpp"
#include "CurODModelItem.hpp"
#include "Cycle.hpp"
#include "EPLVizDefines.hpp"
#include "Packet.hpp"
#include "TreeModelBase.hpp"

#include <QList>
#include <QTreeWidget>
#include <plf_colony.h>
#include <unordered_map>

namespace EPL_Viz {
class CurrentODModel final : public TreeModelBase, public BaseModel {
  Q_OBJECT

 private:
  uint8_t node            = 1;
  uint8_t lastUpdatedNode = node;

 public:
  CurrentODModel(MainWindow *window, QTreeView *widget);
  CurrentODModel() = delete;
  ~CurrentODModel();

  void init() override;

 protected:
  void update(ProtectedCycle &cycle) override;

 public slots:
  void updateNext();
  void changeNode(uint8_t);
  void showContextMenu(const QPoint &);
 signals:
  void drawingPlot(uint8_t nodeID, uint16_t index, uint8_t subIndex);
  void updateExternal(EPL_DataCollect::Cycle *cycle, int node);
};
}
