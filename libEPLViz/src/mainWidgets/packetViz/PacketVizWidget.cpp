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

#include "PacketVizWidget.hpp"
#include "PacketVizModel.hpp"

using namespace EPL_Viz;

PacketVizWidget::PacketVizWidget(QWidget *parent) : QWidget(parent) {
  parentWidget = parent;

  scaleEngine = new QwtLinearScaleEngine();
  scaleWidget = new QwtScaleWidget(QwtScaleDraw::TopScale, parent);

  scaleWidget->setScaleDiv(scaleEngine->divideScale(0, 1000, 10, 10));
  scaleWidget->setBorderDist(3, 3);
  scaleWidget->setTitle("Time passed in us");
  scaleWidget->show();
}

PacketVizWidget::~PacketVizWidget() {}

void PacketVizWidget::redraw() {
  scaleWidget->setGeometry(0, 0, parentWidget->size().width(), 30);
  scaleWidget->setScaleDiv(scaleEngine->divideScale(0, maxTime, 10, 10));
}


void PacketVizWidget::resizeEvent(QResizeEvent *ev) {
  QWidget::resizeEvent(ev);
  redraw();
}

void PacketVizWidget::timeIndexChanged(int index) {
  if (model)
    model->timeIndexChanged(index);
}

void PacketVizWidget::setModel(PacketVizModel *m) { model = m; }
void PacketVizWidget::setMaxTime(int t) { maxTime = t; }
