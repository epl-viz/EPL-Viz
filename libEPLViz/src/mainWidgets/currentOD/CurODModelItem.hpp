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
 * \file CurODModelItem.hpp
 */

#pragma once
#include "BaseModel.hpp"
#include "TreeModelItemBase.hpp"
#include <memory>
#include <stdint.h>
#include <unordered_map>
#include <vector>

namespace EPL_Viz {

class CurODModelItem : public TreeModelItemBase {
 private:
  ProtectedCycle &c;
  uint8_t         node     = UINT8_MAX;
  uint16_t        index    = UINT16_MAX;
  uint16_t        subIndex = UINT16_MAX; // Values bigger UINT8_MAX: this is not a sub index
  bool            hasColor = false;
  QColor          color;

  QVariant dataDisplay(int column);
  QVariant dataTooltip(int column);
  QVariant dataBackground(int colummn);

 public:
  CurODModelItem() = delete;
  CurODModelItem(TreeModelItemBase *parent,
                 ProtectedCycle &   cycle,
                 uint8_t            cNode,
                 uint16_t           odIndex,
                 uint16_t           odSubIndex = UINT16_MAX,
                 bool               isColor    = false,
                 QColor             col        = QColor());

  virtual ~CurODModelItem();

  QVariant data(int column, Qt::ItemDataRole role) override;
  bool          hasChanged() override;
  Qt::ItemFlags flags() override;

  void resetColor();
  void setColor(QColor col);

  uint16_t getIndex() const;
  uint16_t getSubIndex() const;
};
}
