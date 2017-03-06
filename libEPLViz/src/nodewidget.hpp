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
 * \file nodewidget.hpp
 * \brief Represents a node in the network graph.
 *
 * The representation shows the assigned name, device type, status and optionally the most often requested OD Entries of
 * the node.
 *
 * \todo Complete signals, slots and add clicked() signal to notify all other parts of the gui of the node selection.
 */

#pragma once

#include "EPLEnum2Str.hpp"
#include "Node.hpp"
#include "EPLEnums.h"
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
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
  const QString style = QString("#node%1 { background-color: %2; }");

  const QString statusFormat = QString("Status: %1"); //%1 is the status string
  const QString nameFormat = QString("%1 #%2");        //%1 is the node name, %2 its ID

  const QSize maxSize = QSize(300, 300);
  const QSize minSize = QSize(100, 100);
  // clang-format on

  uint8_t                   id;
  QString                   idString;
  QString                   name;
  QString                   device;
  EPL_DataCollect::NMTState status;

  QWidget *     minWidget;
  QWidget *     maxWidget;
  QFrame *      line;
  QLabel *      nameLabel;
  QLabel *      statusLabel;
  QTreeWidget * advancedInfo;
  QLabel *      typeLabel;
  QPushButton * minimizeButton;
  QRadioButton *advanced;

  bool isMinimized = true;

 public:
  NodeWidget(EPL_DataCollect::Node *node, QWidget *parent = nullptr);
  ~NodeWidget() = default;

  static QString statusToBackground(EPL_DataCollect::NMTState status);

  void updateData(EPL_DataCollect::Node *node);

 private:
  void updateAdvancedInfo(EPL_DataCollect::Node::IDENT identity);
  void updateStatus(EPL_DataCollect::NMTState newStatus);

 public slots:
  void minimizeChange(bool minimized);
};
