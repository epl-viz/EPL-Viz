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
 * \file TreeModelItemBase.cpp
 */

#include "TreeModelItemBase.hpp"
using namespace EPL_Viz;
using namespace EPL_DataCollect;

TreeModelItemBase::TreeModelItemBase(TreeModelItemBase *parent) : p(parent) {}
TreeModelItemBase::~TreeModelItemBase() {}

TreeModelRoot::~TreeModelRoot() {}


TreeModelItemBase *TreeModelItemBase::parent() { return p; }

TreeModelItemBase *TreeModelItemBase::child(size_t row) {
  if (row > childItems.size())
    return nullptr;

  return childItems[row].get();
}

void TreeModelItemBase::push_back(std::unique_ptr<TreeModelItemBase> item) {
  childItems.emplace_back(std::move(item));
  childIndexMap[childItems.back().get()] = childItems.size() - 1;
}

void TreeModelItemBase::clear() {
  childItems.clear();
  childIndexMap.clear();
}

TreeModelItemBase::LIST *TreeModelItemBase::getChildren() { return &childItems; }
int                      TreeModelItemBase::childCount() { return static_cast<int>(childItems.size()); }


size_t TreeModelItemBase::indexOf(TreeModelItemBase const *item) {
  if (childIndexMap.find(item) == childIndexMap.end()) {
    qDebug() << "DATA STRUCTURE ERROR: CAN NOT CALCULTE INDEX OF ITEM";
    return 0;
  }

  return childIndexMap.at(item);
}

size_t TreeModelItemBase::row() {
  if (!p)
    return 0;

  return p->indexOf(this);
}

int TreeModelItemBase::columnCount() const {
  if (!p)
    return 0;

  return p->columnCount();
}


QVariant TreeModelRoot::headerData(int section, Qt::Orientation orientation, int role) {
  if (orientation != Qt::Horizontal)
    return QVariant();

  auto &vec = hData[role];
  if (section < 0 || section >= static_cast<int>(vec.size()))
    return QVariant();

  return vec[static_cast<size_t>(section)];
}

int TreeModelRoot::columnCount() const {
  if (hData.find(Qt::DisplayRole) == hData.end())
    return 0;

  return static_cast<int>(hData.at(Qt::DisplayRole).size());
}
