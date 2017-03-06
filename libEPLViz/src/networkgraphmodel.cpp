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
 * \file networkgraphmodel.cpp
 */

#include "networkgraphmodel.hpp"
#include "mainwindow.hpp"

using namespace EPL_Viz;
using namespace EPL_DataCollect;

NetworkGraphModel::NetworkGraphModel(MainWindow *mw) { graph = mw->getNetworkGraph(); }

NetworkGraphModel::~NetworkGraphModel() {}

void NetworkGraphModel::init() {}

void NetworkGraphModel::update(Cycle *cycle) {
  auto list = cycle->getNodeList();

  for (uint8_t id : list) {
    Node *n = cycle->getNode(id);
    auto  s = nodeMap.find(id);

    if (s == nodeMap.end() || s.key() != id) {
      // The node is not yet added as a widget and has to be created
      NodeWidget *nw = new NodeWidget(n, graph);
      nodeMap.insert(id, nw);
      graph->layout()->addWidget(nw);
    } else {
      // The node is added as widget and has to be updated
      nodeMap[id]->updateData(n);
    }
  }
}
