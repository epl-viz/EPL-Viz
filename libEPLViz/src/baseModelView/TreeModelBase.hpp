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
 * \file TreeModelBase.hpp
 */
#pragma once

#include "BaseModel.hpp"
#include "Cycle.hpp"
#include "EPLVizDefines.hpp"
#include "Packet.hpp"
#include "TreeModelItemBase.hpp"

#include <QTreeWidget>
#include <mutex>
#include <plf_colony.h>
#include <unordered_map>
#include <vector>

namespace EPL_Viz {
class TreeModelBase : public QAbstractItemModel {
  Q_OBJECT

 protected:
  TreeModelItemBase *  root = nullptr;
  std::recursive_mutex mutex;

  std::unique_lock<std::recursive_mutex> getLock() const;

 public:
  TreeModelBase() = delete;
  TreeModelBase(QAbstractItemView *widget);
  virtual ~TreeModelBase();

  TreeModelItemBase *getItem(const QModelIndex &index) const;

  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  int rowCount(const QModelIndex &parent) const override;
  int columnCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
  Qt::ItemFlags flags(const QModelIndex &index) const override;

  QModelIndex indexOf(TreeModelItemBase *item, int column = 0) const;
  QModelIndex parentOf(TreeModelItemBase *item, int column = 0) const;

  void emitRowChaned(QModelIndex index);
  void emitRowChaned(TreeModelItemBase *item);
};
}
