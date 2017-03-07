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
 * \file curodmodelitem.hpp
 */

#pragma once
#include <QMap>
#include <memory>
#include <stdint.h>

namespace EPL_Viz {
class CurODModelItem {
 public:
  CurODModelItem(
        uint16_t index, bool hasSub, bool isSub, QString data, std::shared_ptr<CurODModelItem> parent = nullptr);

  bool     hasSubIndex();
  bool     isSubIndex();
  uint16_t getIndex();
  QString  getData();
  bool setSubIndex(uint16_t i, std::shared_ptr<CurODModelItem> item);
  std::shared_ptr<CurODModelItem> getSubindex(uint16_t i);
  std::shared_ptr<CurODModelItem> getParent();

 private:
  bool                            hasSub;
  bool                            isSub;
  uint16_t                        index;
  QString                         data;
  std::shared_ptr<CurODModelItem> parent;

  QMap<uint16_t, std::shared_ptr<CurODModelItem>> subIndices;
};
}
