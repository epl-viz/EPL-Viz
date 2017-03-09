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
 * \file TreeModelItemBase.hpp
 */

#pragma once
#include "BaseModel.hpp"
#include <memory>
#include <stdint.h>
#include <unordered_map>
#include <vector>

namespace EPL_Viz {

class TreeModelItemBase {
 public:
  typedef std::vector<std::unique_ptr<TreeModelItemBase>> LIST;
  typedef std::unordered_map<TreeModelItemBase const *, size_t> MAP;

 private:
  TreeModelItemBase *p = nullptr;
  LIST               childItems;
  MAP                childIndexMap;

 public:
  TreeModelItemBase() = delete;
  TreeModelItemBase(TreeModelItemBase *parent);

  virtual ~TreeModelItemBase();

  TreeModelItemBase *parent();
  TreeModelItemBase *child(size_t row);
  int    childCount() const;
  size_t row() const;
  size_t indexOf(TreeModelItemBase const *item) const;

  virtual QVariant data(int column, Qt::ItemDataRole role) = 0;
  virtual bool          hasChanged() = 0;
  virtual Qt::ItemFlags flags()      = 0;
  virtual int           columnCount() const;

  LIST *getChildren();
  void push_back(std::unique_ptr<TreeModelItemBase> item);
  void clear();
};

class TreeModelRoot : public TreeModelItemBase {
 private:
  std::unordered_map<int, std::vector<QVariant>> hData;

 public:
  TreeModelRoot(std::unordered_map<int, std::vector<QVariant>> headerData)
      : TreeModelItemBase(nullptr), hData(headerData) {}
  virtual ~TreeModelRoot();

  QVariant headerData(int section, Qt::Orientation orientation, int role);
  int columnCount() const override;

  QVariant      data(int, Qt::ItemDataRole) override { return QVariant(); }
  bool          hasChanged() override { return false; }
  Qt::ItemFlags flags() override { return 0; }
};
}
