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

#include "PacketVizPacket.hpp"
#include "EPLEnum2Str.hpp"
#include "PacketVizWidget.hpp"
#include "ui_PacketVizPacket.h"
#include <QPainter>

using namespace EPL_Viz;
using namespace EPL_DataCollect;

PacketVizPacket::PacketVizPacket(QWidget *parent) : QFrame(parent), ui(new Ui::PacketVizPacket) {
  ui->setupUi(this);
  parentWidget = dynamic_cast<PacketVizWidget *>(parent);

  if (!parentWidget)
    qDebug() << "ERROR: !!! INVALID PARENT !!!";
}

PacketVizPacket::~PacketVizPacket() { delete ui; }
void PacketVizPacket::mousePressEvent(QMouseEvent *) { parentWidget->packetSelected(pkgIndex); }

QColor PacketVizPacket::calcBGColor(EPL_DataCollect::PacketType type, SettingsProfileItem::Config &cfg) {
  switch (type) {
    case PacketType::START_OF_CYCLE: return cfg.pSoC;
    case PacketType::START_OF_ASYNC: return cfg.pSoA;
    case PacketType::POLL_REQUEST: return cfg.pPReq;
    case PacketType::POLL_RESPONSE: return cfg.pPRes;
    case PacketType::ASYNC_SEND: return cfg.pASnd;
    case PacketType::AINV: return cfg.pAINV;
    case PacketType::AMNI: return cfg.pANMI;
    case PacketType::UNDEF: return cfg.pInvalid;
  }

  return QColor();
}

void PacketVizPacket::setPacketData(InputHandler::PacketMetadata pData,
                                    int                          relTime,
                                    uint64_t                     pIndex,
                                    SettingsProfileItem::Config &cfg) {
  pkgIndex          = pIndex;
  PacketType pType  = static_cast<PacketType>(pData.getFiled(InputHandler::PacketMetadata::PACKET_TYPE));
  int        source = static_cast<int>(pData.getFiled(InputHandler::PacketMetadata::SOURCE));
  int        dest   = static_cast<int>(pData.getFiled(InputHandler::PacketMetadata::DESTINATION));

  QColor   bgColor = calcBGColor(pType, cfg);
  QPalette pal     = palette();

  pal.setBrush(QPalette::Window, QBrush(bgColor, Qt::DiagCrossPattern));
  pal.setColor(QPalette::WindowText, bgColor.lightness() >= 125 ? QColor("#000000") : QColor("#ffffff"));

  setPalette(pal);

  pal.setBrush(QPalette::Window, QBrush(bgColor, Qt::SolidPattern));
  ui->baseBack->setPalette(pal);

  QString l1 = EPLEnum2Str::toStr(static_cast<PacketType>(pType)).c_str();
  QString l2 = QString::fromStdString(std::to_string(source)) + " --> " + std::to_string(dest).c_str();
  QString l3 = QString::fromStdString(std::to_string(relTime)) + " μs";

  if (!cfg.packetVizDisableText) {
    ui->pType->setText(EPLEnum2Str::toStr(static_cast<PacketType>(pType)).c_str());
    ui->pSourceDest->setText(QString::fromStdString(std::to_string(source)) + " --> " + std::to_string(dest).c_str());
    ui->pDur->setText(QString::fromStdString(std::to_string(relTime)) + " μs");
  } else {
    ui->pType->setText("");
    ui->pSourceDest->setText("");
    ui->pDur->setText("");
  }

  setToolTip("PacketType: " + l1 + "\nSource / Dest: " + l2 + "\nDuration: " + l3);
}

void PacketVizPacket::resizeAll(int x, int y) {
  std::vector<QLabel *> sizes = {ui->pType, ui->pSourceDest, ui->pDur};
  int                   minX  = 0;

  for (QLabel *i : sizes) {
    int s = i->fontMetrics().boundingRect(i->text()).width();
    minX  = s > minX ? s : minX;
  }

  minX += ui->container->layout()->margin() * 2;

  resize(x < minX ? minX : x, y);
  ui->container->resize(size());
  ui->baseBack->resize(x, y);
}

void PacketVizPacket::setHMode(HighlightMode m) {
  hMode = m;
  switch (hMode) {
    case NONE: ui->container->setFrameShape(QFrame::NoFrame); break;
    case SELECTED: ui->container->setFrameShape(QFrame::Box); break;
  }
}
