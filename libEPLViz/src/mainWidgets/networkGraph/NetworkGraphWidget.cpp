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

using namespace EPL_Viz;

NetworkGraphWidget::NetworkGraphWidget(QWidget *parent) : QWidget(parent) {}

QMap<uint8_t, NodeWidget *> *NetworkGraphWidget::getNodeWidgets() { return &nodeMap; }


void NetworkGraphWidget::reset() {
  count = 0;

  current = UINT8_MAX;

  for (auto nw : nodeMap.values()) {
    grid->removeWidget(nw);
    delete nw;
  }

  nodeMap.clear();
}

void NetworkGraphWidget::createWidget(uint8_t nID, EPL_Viz::ProtectedCycle &c) {
  // Ensure that a layout is present
  if (!grid) {
    grid = qobject_cast<QGridLayout *>(layout());

    // Create a new one if it is missing
    if (!grid) {
      grid = new QGridLayout();
      this->setLayout(grid);
    }
  }

  auto                   lock = c.getLock();
  EPL_DataCollect::Node *n    = c->getNode(nID);

  qDebug() << "Creating new node " << QString::number(nID);

  if (!n) {
    qDebug() << "Invalid node " << QString::number(nID);
    return;
  }

  NodeWidget *nw = new NodeWidget(mw, n, this);

  connect(nw, SIGNAL(nodeClicked(uint8_t)), this, SLOT(selectNode(uint8_t)));
  nodeMap.insert(nID, nw);

  qDebug() << "Placing new node widget at row " << QString::number(count / 4) << " and column "
           << QString::number(count % 4);

  grid->addWidget(nw, count / 4, count % 4);
  count++;
}

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

  if (current != UINT8_MAX)
    nodeMap[current]->setSelected(true);

  emit nodeSelected(current);
}
