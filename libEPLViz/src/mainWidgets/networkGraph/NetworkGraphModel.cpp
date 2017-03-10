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
 * \file NetworkGraphModel.cpp
 */

#include "NetworkGraphModel.hpp"
#include "MainWindow.hpp"
#include <QDebug>

using namespace EPL_Viz;
using namespace EPL_DataCollect;

NetworkGraphModel::NetworkGraphModel(MainWindow *mw, NetworkGraphWidget *widget) : BaseModel(mw, widget) {
  graph = widget;
  // TODO: Move the connects to the MainWindow::createModels and connect to networkgraphcontents
  connect(this, SIGNAL(detectedNewNode(uint8_t, ProtectedCycle &)), mw, SLOT(addNode(uint8_t, ProtectedCycle &)));
  connect(mw, SIGNAL(nodeAdded(uint8_t, NodeWidget *)), this, SLOT(trackNodeWidget(uint8_t, NodeWidget *)));
}

void NetworkGraphModel::init() {}

void NetworkGraphModel::update(ProtectedCycle &cycle) {
  auto lock = cycle.getLock();
  auto list = cycle->getNodeList();

  for (uint8_t id : list) {
    auto s = nodeMap.find(id);

    if (s == nodeMap.end() || s.key() != id) {
      // The node is not yet added as a widget and has to be created
      emit detectedNewNode(id, cycle);
    } else {
      // The node is added as widget and has to be updated
      emit nodeUpdated(id, cycle);
      // Reset highlighting
      nodeMap[id]->setHighlightingLevel(0);
    }
  }

  auto events = cycle->getActiveEvents();

  // Iterate over all events that are currently active
  for (auto event : events) {
    switch (event->getType()) {
      case EvType::VIEW_EV_HIGHLIGHT_MN:
      case EvType::VIEW_EV_HIGHLIGHT_CN: {
        uint64_t node  = event->getEventFlags();             // EventFlags is the NodeID for these events
        int      level = std::stoi(event->getDescription()); // Description is the highlighting level for these events

        // Invalid node
        if (cycle->getNode(static_cast<uint8_t>(node)) == nullptr)
          break;

        // Check if event is valid
        if ((event->getType() == EvType::VIEW_EV_HIGHLIGHT_CN && node < 240) ||
            (event->getType() == EvType::VIEW_EV_HIGHLIGHT_CN && node == 240)) {
          // Set highlighting
          nodeMap[static_cast<uint8_t>(node)]->setHighlightingLevel(level);
        }
        break;
      }
      default: break;
    }
  }

  emit eventsDone();
}

void NetworkGraphModel::trackNodeWidget(uint8_t id, NodeWidget *nw) {
  nodeMap.insert(id, nw);
  connect(this,
          SIGNAL(nodeUpdated(uint8_t nID, ProtectedCycle & c)),
          nw,
          SLOT(updateData(uint8_t nID, ProtectedCycle & c)));
  connect(this, SIGNAL(eventsDone()), nw, SLOT(updateStyleSheet()));
}
