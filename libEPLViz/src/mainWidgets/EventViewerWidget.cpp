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
 * \file EventViewerWidget.cpp
 */

#include "EventViewerWidget.hpp"
#include "MainWindow.hpp"
#include <QHeaderView>

using namespace EPL_Viz;
using namespace EPL_DataCollect;

EventViewerWidget::EventViewerWidget(QWidget *parent) : QTreeWidget(parent) {
  connect(this, SIGNAL(itemActivated(QTreeWidgetItem *, int)), this, SLOT(jumpToEvent(QTreeWidgetItem *)));
  connect(this, SIGNAL(itemClicked(QTreeWidgetItem *, int)), this, SLOT(jumpToEvent(QTreeWidgetItem *)));
  header()->setSectionsMovable(false);
}

void EventViewerWidget::reset() {
  // Delete all entries
  while (topLevelItemCount() > 0) {
    delete takeTopLevelItem(0);
  }

  log   = nullptr;
  appID = UINT32_MAX;
}

void EventViewerWidget::jumpToEvent(QTreeWidgetItem *item) {
  uint32_t cycle = static_cast<uint32_t>(std::atoi(item->text(4).toStdString().c_str()));

  emit eventSelected(cycle);
}

void EventViewerWidget::start(CaptureInstance *ci) {
  log   = ci->getEventLog();
  appID = log->getAppID();
}

void EventViewerWidget::updateEvents() {
  // Don't update if the widget is not ready yet
  if (!isEnabled() || !log)
    return;

  auto events = log->pollEvents(appID);
  for (auto event : events) {
    QTreeWidgetItem *evItem = new QTreeWidgetItem(this);

    uint32_t first = 0; // The first cycle this event occured in
    uint32_t last  = 0; // The last cycle this event occured in

    // Load the values for first and last
    event->getCycleRange(&first, &last);

    evItem->setText(0, QString::fromStdString(event->getName()));         // The name of the event
    evItem->setText(1, QString::fromStdString(event->getDescription()));  // The description of the event
    evItem->setText(2, QString::fromStdString(event->getTypeAsString())); // The type of the event
    evItem->setText(3, QString::number(event->getEventFlags()));          // The flags of the event
    evItem->setText(4, QString::number(first));                           // The first cycle occurrence of the event
    evItem->setText(5, QString::number(last));                            // The last cycle occurrence of the event
    evItem->setText(6, QString::fromStdString(event->getPluginID()));     // The name of the plugin that sent the event

    QColor evColor;
    switch (event->getType()) {
      case EvType::PROTO_ERROR: evColor    = mw->getSettingsWin()->getConfig().evProtoError; break;
      case EvType::ERROR: evColor          = mw->getSettingsWin()->getConfig().evError; break;
      case EvType::WARNING: evColor        = mw->getSettingsWin()->getConfig().evWarning; break;
      case EvType::INFO: evColor           = mw->getSettingsWin()->getConfig().evInfo; break;
      case EvType::DEBUG: evColor          = mw->getSettingsWin()->getConfig().evDebug; break;
      case EvType::PLUGIN_EV_TEXT: evColor = mw->getSettingsWin()->getConfig().evPText; break;
      default: break;
    }

    if (evColor.isValid()) {
      QColor evFornt = evColor.lightness() >= 125 ? "#000000" : "#ffffff";
      for (int i = 0; i < 7; ++i) {
        evItem->setBackground(i, evColor);
        evItem->setForeground(i, evFornt);
      }
    }
  }
}
