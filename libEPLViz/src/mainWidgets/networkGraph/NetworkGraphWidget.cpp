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
 * \file NetworkGraphWidget.cpp
 */

#include "NetworkGraphWidget.hpp"
#include "Node.hpp"

NetworkGraphWidget::NetworkGraphWidget(QWidget *parent) : QWidget(parent) {
  grid = qobject_cast<QGridLayout *>(layout());

  // Ensure that a layout is present
  if (!grid) {
    grid = new QGridLayout();
    this->setLayout(grid);
  }
}

QMap<uint8_t, NodeWidget *> *NetworkGraphWidget::getNodeWidgets() { return &nodeMap; }


void NetworkGraphWidget::updateWidget(EPL_Viz::ProtectedCycle &c) {
  auto lock = c.getLock();

  // TODO: Use an alternative for hiding unupdated nodes?
  QMap<uint8_t, NodeWidget *> nodes(nodeMap); // Used to track untouched nodes

  // Apply all queued updates
  for (auto nID : updateQueue) {
    NodeWidget *nw = nodeMap[nID];

    nw->updateData(nID, c);
    nw->show(); // Show the widget if it was hidden

    nodes.remove(nID);
  }

  updateQueue.clear();

  // Create queued widgets
  for (auto nID : createQueue) {
    EPL_DataCollect::Node *n = c->getNode(nID);

    qDebug() << "Creating new node " << QString::number(nID);

    if (!n) {
      qDebug() << "Invalid node " << QString::number(nID);
      return;
    }

    NodeWidget *nw = new NodeWidget(n, this);

    connect(nw, SIGNAL(nodeClicked(uint8_t)), this, SLOT(selectNode(uint8_t)));
    nodeMap.insert(nID, nw);

    qDebug() << "Placing new node widget at row " << QString::number(count / 4) << " and column "
             << QString::number(count % 4);

    grid->addWidget(nw, count / 4, count % 4);
    count++;
  }

  createQueue.clear();

  // Hide widgets that were not updated (Allows jumps back in time)
  for (auto nw : nodes.values()) {
    // Unselect the node if it is hidden and notify others
    if (nw->isSelected()) {
      nw->setSelected(false);
      current = UINT8_MAX;
      emit nodeSelected(current);
    }
    nw->hide();
  }
}

void NetworkGraphWidget::queueNodeUpdate(uint8_t node) { updateQueue.append(node); }

void NetworkGraphWidget::queueNodeCreation(uint8_t node) { createQueue.append(node); }

void NetworkGraphWidget::selectNode(uint8_t node) {
  // Check if there is currently a selected node
  if (current != UINT8_MAX) {
    // Check if the same node is trying to be selected
    if (current == node)
      return;

    // Unselect old node
    nodeMap[current]->setSelected(false);
  }

  // Update current node
  current = node;
  nodeMap[current]->setSelected(true);
  emit nodeSelected(current);
}
