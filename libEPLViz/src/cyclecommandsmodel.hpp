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
 * \file cyclecommandsmodel.hpp
 * Adapted from https://doc.qt.io/qt-5/qtwidgets-itemviews-editabletreemodel-example.html
 */
#pragma once

#include "basemodel.hpp"
#include "cycotreeitem.hpp"
#include <QAbstractItemModel>
#include <QTreeView>

namespace EPL_Viz {
class CycleCommandsModel : public QAbstractItemModel, public BaseModel {
  Q_OBJECT

 public:
  CycleCommandsModel(QObject *parent = 0);
  ~CycleCommandsModel();

  QVariant data(const QModelIndex &index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  int rowCount(const QModelIndex &parent = QModelIndex()) const override;
  int columnCount(const QModelIndex &parent = QModelIndex()) const override;

  Qt::ItemFlags flags(const QModelIndex &index) const override;
  // bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
  // bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole)
  // override;
  bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;
  bool removeRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;

  void init() override;

 private:
  uint8_t       selectedNode;
  bool          needUpdate;
  CyCoTreeItem *rootItem;
  QTreeView *   view;

  CyCoTreeItem *getItem(const QModelIndex &index) const;

 protected:
  mockable void update(EPL_DataCollect::Cycle *cycle) override;

 public slots:
  void updateNext();
  void changeNode(uint8_t newNode);
};
}