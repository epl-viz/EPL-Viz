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
 * \file currentodmodel.hpp
 */
#pragma once

#include "Cycle.hpp"
#include "EPLVizDefines.hpp"
#include "Packet.hpp"
#include "basemodel.hpp"
#include "curodmodelitem.hpp"

#include <QAbstractItemModel>
#include <QHeaderView>
#include <QTableView>
#include <unordered_map>

namespace EPL_Viz {
class CurrentODModel : public QAbstractItemModel, public BaseModel {
  Q_OBJECT
 private:
  uint8_t node = 1;
  bool    needUpdate;
  QMap<uint16_t, std::shared_ptr<CurODModelItem>> odEntries;
  QMap<int, std::pair<uint16_t, uint16_t>> convertRow;

  std::shared_ptr<CurODModelItem> getItem(const QModelIndex &index) const;
  std::shared_ptr<CurODModelItem> getItem(int row) const;

 public:
  CurrentODModel(QMainWindow *window);
  ~CurrentODModel() = default;
  void init() override;

  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
  QModelIndex parent(const QModelIndex &index) const override;
  int rowCount(const QModelIndex &parent) const override;
  int columnCount(const QModelIndex &parent) const override;
  QVariant data(const QModelIndex &index, int role) const override;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

 protected:
  mockable void update(EPL_DataCollect::Cycle *cycle) override;

 public slots:
  void updateNext();
  void changeNode(uint8_t);
};
}
