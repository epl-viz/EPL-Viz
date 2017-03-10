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
 * \file NodeWidget.hpp
 * \brief Represents a node in the network graph.
 *
 * The representation shows the assigned name, device type, status and optionally the most often requested OD Entries of
 * the node.
 */

#pragma once

#include "BaseModel.hpp"
#include "EPLEnum2Str.hpp"
#include "Node.hpp"
#include "EPLEnums.h"
#include <QDebug>
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QRadioButton>
#include <QSize>
#include <QStackedWidget>
#include <QString>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <QWidget>

class NodeWidget : public QStackedWidget {
  Q_OBJECT

 private:
  // clang-format off
  const QString styleFormat = QString("#node-%1%2 { background-color: %3; border-color: rgb(%4, 0, 0); border-style: outset;  border-width: 2px;}"); //TODO: Add border for stylesheet

  const QString statusFormat = QString("Status: %1"); //%1 is the status string
  const QString nameFormat = QString("%1 #%2");        //%1 is the node name, %2 its ID

  const QSize maxSizeMax = QSize(300, 300);
  const QSize maxSizeMin = QSize(300, 100);
  const QSize minSize = QSize(150, 150);
  // clang-format on

  uint8_t                   id;
  QString                   idString;
  EPL_DataCollect::NMTState status;

  int highlightingLevel = 0;

  QWidget *     minWidget;
  QWidget *     maxWidget;
  QFrame *      line;
  QLabel *      nameLabelMax;
  QLabel *      nameLabelMin;
  QLabel *      statusLabel;
  QTreeWidget * advancedInfo;
  QLabel *      typeLabel;
  QPushButton * minimizeButton;
  QPushButton * maximizeButton;
  QRadioButton *advanced;

  bool isMinimized = false;

 public:
  NodeWidget(EPL_DataCollect::Node *node, QWidget *parent = nullptr);
  ~NodeWidget() = default;

  static QString statusToBackground(EPL_DataCollect::NMTState status);

 private:
  void updateIdentityInfo(EPL_DataCollect::Node::IDENT identity);
  void updateStatus(EPL_DataCollect::NMTState newStatus);

  QString validateUInt(uint16_t val);
  QString validateUInt(uint32_t val);

 protected:
  void mousePressEvent(QMouseEvent *event) override;

 signals:
  void nodeChanged(uint8_t node);

 private slots:
  void fixTree(QTreeWidgetItem *item);

 public slots:
  void minimizeChange(bool minimized);
  void setHighlightingLevel(int level);
  void updateData(uint8_t nID, EPL_Viz::ProtectedCycle &c);
  void updateStyleSheet();
};
