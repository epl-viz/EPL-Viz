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

#pragma once

#include "CaptureInstance.hpp"
#include "PlotCreator.hpp"
#include <QListWidget>
#include <unordered_map>

namespace EPL_Viz {

class SettingsWindow;

class SettingsProfileItem final : public QListWidgetItem {
 public:
  struct Config {
    EPL_DataCollect::CaptureInstance::Config backConf;
    std::unordered_map<int, EPL_DataCollect::CaptureInstance::NodeConfig> nodes;
    std::string               pythonPluginsDir;
    int                       currentNode;
    bool                      pauseWhilePlayingFile = true;
    bool                      invertTimeLineZoom    = true;
    std::chrono::milliseconds guiThreadWaitTime     = std::chrono::milliseconds(100);

    std::vector<PlotCreator::PlotCreatorData> plots;

    QColor odHighlight = QColor("#14aaff");
    QColor pInvalid    = QColor("#801515");
    QColor pSoC        = QColor("#116611");
    QColor pSoA        = QColor("#55AA55");
    QColor pPRes       = QColor("#804515");
    QColor pPReq       = QColor("#D49A6A");
    QColor pASnd       = QColor("#D46A6A");
    QColor pAINV       = QColor("#0D4D4D");
    QColor pANMI       = QColor("#407F7F");

    QColor evProtoError = QColor("#550000");
    QColor evError      = QColor("#801515");
    QColor evWarning    = QColor("#806415");
    QColor evInfo       = QColor("#116611");
    QColor evDebug      = QColor("#55AA55");
    QColor evPText      = QColor("#0D4D4D");

    QColor NMT_Highlighted         = QColor("#B71C1C");
    QColor NMT_OFF                 = QColor("#9E9E9E");
    QColor NMT_INITIALISING        = QColor("#00BCD4");
    QColor NMT_RESET_APPLICATION   = QColor("#009688");
    QColor NMT_RESET_COMMUNICATION = QColor("#009688");
    QColor NMT_RESET_CONFIGURATION = QColor("#009688");
    QColor NMT_NOT_ACTIVE          = QColor("#607D8B");
    QColor NMT_PRE_OPERATIONAL_1   = QColor("#FFEB3B");
    QColor NMT_PRE_OPERATIONAL_2   = QColor("#FFC107");
    QColor NMT_READY_TO_OPERATE    = QColor("#2196F3");
    QColor NMT_OPERATIONAL         = QColor("#8BC34A");
    QColor NMT_STOPPED             = QColor("#F44336");
    QColor NMT_BASIC_ETHERNET      = QColor("#9C27B0");

    Config();
  };

 private:
  Config  cfg;
  QString name;

 public:
  SettingsProfileItem(QString str, QListWidget *parent);
  ~SettingsProfileItem();

  Config  getConfig();
  QString getName();
  friend class SettingsWindow;
};
}
