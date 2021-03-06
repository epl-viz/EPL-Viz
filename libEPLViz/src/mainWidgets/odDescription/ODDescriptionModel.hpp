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
 * \file ODDescriptionModel.hpp
 */
#pragma once

#include "BaseModel.hpp"
#include "CurODModelItem.hpp"
#include "Cycle.hpp"
#include "EPLVizDefines.hpp"
#include "ODFilterDesc.hpp"
#include "Packet.hpp"
#include "TreeModelBase.hpp"

#include <QList>
#include <QTreeWidget>
#include <plf_colony.h>
#include <unordered_map>

namespace EPL_Viz {
class ODDescriptionModel final : public TreeModelBase, public BaseModel {
  Q_OBJECT
 private:
  uint8_t node       = UINT8_MAX;
  bool    hasChanged = false;

  ODFilterDesc *filter = nullptr;

 public:
  ODDescriptionModel(MainWindow *window, QTreeView *treeWidget);
  ODDescriptionModel()  = delete;
  ~ODDescriptionModel() = default;

  QString getName() override { return "ODDescriptionModel"; }


 protected:
  void init() override;
  void update() override;
  void updateWidget() override;
  bool needUpdateAlways() override;

 public slots:
  void selectNode(uint8_t);
};
}
