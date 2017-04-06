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

#include "SettingsProfileItem.hpp"
#include <QDir>
#include <QSettings>
#include <QStandardPaths>

using namespace EPL_Viz;

SettingsProfileItem::~SettingsProfileItem() {}

SettingsProfileItem::SettingsProfileItem(QString str, QListWidget *parent) : QListWidgetItem(str, parent), name(str) {}

SettingsProfileItem::Config SettingsProfileItem::getConfig() { return cfg; }

SettingsProfileItem::Config::Config() {
  pythonPluginsDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation).toStdString();
  QDir dir(pythonPluginsDir.c_str());
  if (!dir.exists()) {
    dir.mkpath(pythonPluginsDir.c_str());
  }
}

QString SettingsProfileItem::getName() { return name; }

bool SettingsProfileItem::exportProf(QString file) {
  QSettings settings(file, QSettings::IniFormat, nullptr);

  settings.setValue("EPL_DC/xddDir", cfg.backConf.xddDir.c_str());
  settings.setValue("pauseWhilePlayingFile", cfg.pauseWhilePlayingFile);
  settings.setValue("invertTimeLineZoom", cfg.invertTimeLineZoom);
  settings.setValue("guiThreadWaitTime", static_cast<int>(cfg.guiThreadWaitTime.count()));
  settings.setValue("EPL_DC/SM/saveInterval", cfg.backConf.smConfig.saveInterval);
  settings.setValue("pythonPluginsDir", cfg.pythonPluginsDir.c_str());
  settings.setValue("EPL_DC/IH/eplFrameName", cfg.backConf.ihConfig.eplFrameName.c_str());
  settings.setValue("EPL_DC/IH/prefetchSize", cfg.backConf.ihConfig.prefetchSize);
  settings.setValue("EPL_DC/IH/cleanupInterval", cfg.backConf.ihConfig.cleanupInterval);
  settings.setValue("EPL_DC/IH/checkPrefetch", cfg.backConf.ihConfig.checkPrefetch);
  settings.setValue("EPL_DC/IH/deleteCyclesAfter", static_cast<int>(cfg.backConf.ihConfig.deleteCyclesAfter.count()));
  settings.setValue("EPL_DC/IH/loopWaitTimeout", static_cast<int>(cfg.backConf.ihConfig.loopWaitTimeout.count()));

  std::vector<std::pair<QString, QColor *>> colorSelctor = {
        {"COL_odHighlight", &cfg.odHighlight},
        {"COL_PASnd", &cfg.pASnd},
        {"COL_pInvalid", &cfg.pInvalid},
        {"COL_pPReq", &cfg.pPReq},
        {"COL_pPRes", &cfg.pPRes},
        {"COL_pSoA", &cfg.pSoA},
        {"COL_pSoC", &cfg.pSoC},
        {"COL_ANMI", &cfg.pANMI},
        {"COL_AINV", &cfg.pAINV},

        {"COL_ProtoError", &cfg.evProtoError},
        {"COL_Error", &cfg.evError},
        {"COL_Warning", &cfg.evWarning},
        {"COL_Info", &cfg.evInfo},
        {"COL_Debug", &cfg.evDebug},
        {"COL_PText", &cfg.evPText},

        {"COL_nHighlighted", &cfg.NMT_Highlighted},
        {"COL_NMTOFF", &cfg.NMT_OFF},
        {"COL_INITIALISING", &cfg.NMT_INITIALISING},
        {"COL_RESET_APPLICATION", &cfg.NMT_RESET_APPLICATION},
        {"COL_RESET_COMMUNICATION", &cfg.NMT_RESET_COMMUNICATION},
        {"COL_RESET_CONFIGURATION", &cfg.NMT_RESET_CONFIGURATION},
        {"COL_NOT_ACTIVE", &cfg.NMT_NOT_ACTIVE},
        {"COL_PRE_OPERATIONAL_1", &cfg.NMT_PRE_OPERATIONAL_1},
        {"COL_PRE_OPERATIONAL_2", &cfg.NMT_PRE_OPERATIONAL_2},
        {"COL_READY_TO_OPERATE", &cfg.NMT_READY_TO_OPERATE},
        {"COL_OPERATIONAL", &cfg.NMT_OPERATIONAL},
        {"COL_STOPPED", &cfg.NMT_STOPPED},
        {"COL_BASIC_ETHERNET", &cfg.NMT_BASIC_ETHERNET},
  };

  for (auto i : colorSelctor) {
    if (i.second->isValid()) {
      settings.setValue(i.first, i.second->name());
    } else {
      settings.setValue(i.first, "<NOT SET>");
    }
  }

  settings.beginWriteArray("nodes");
  int counter = 0;
  for (auto i : cfg.nodes) {
    settings.setArrayIndex(counter);
    settings.setValue("id", i.first);
    settings.setValue("autoDeduceSpecificProfile", i.second.autoDeduceSpecificProfile);
    settings.setValue("baseProfile", i.second.baseProfile.c_str());
    settings.setValue("specificProfile", i.second.specificProfile.c_str());
    ++counter;
  }
  settings.endArray();

  settings.beginWriteArray("plots");
  counter = 0;
  for (auto i : cfg.plots) {
    settings.setArrayIndex(counter);
    settings.setValue("defaultODPlot", i.defaultODPlot);
    settings.setValue("node", static_cast<int>(i.node));
    settings.setValue("index", static_cast<int>(i.index));
    settings.setValue("subIndex", static_cast<int>(i.subIndex));
    settings.setValue("addToTimeLine", i.addToTimeLine);
    settings.setValue("addToPlot", i.addToPlot);
    settings.setValue("csName", i.csName.c_str());
    settings.setValue("color", i.color.name());
    ++counter;
  }
  settings.endArray();
  settings.sync();
  return true;
}

bool SettingsProfileItem::importProf(QString file) {
  QSettings settings(file, QSettings::IniFormat, nullptr);

  cfg.backConf.xddDir                   = settings.value("EPL_DC/xddDir").toString().toStdString();
  cfg.pauseWhilePlayingFile             = settings.value("pauseWhilePlayingFile").toBool();
  cfg.invertTimeLineZoom                = settings.value("invertTimeLineZoom").toBool();
  cfg.guiThreadWaitTime                 = std::chrono::milliseconds(settings.value("guiThreadWaitTime").toInt());
  cfg.backConf.smConfig.saveInterval    = static_cast<uint32_t>(settings.value("EPL_DC/SM/saveInterval").toInt());
  cfg.pythonPluginsDir                  = settings.value("pythonPluginsDir").toString().toStdString();
  cfg.backConf.ihConfig.eplFrameName    = settings.value("EPL_DC/IH/eplFrameName").toString().toStdString();
  cfg.backConf.ihConfig.prefetchSize    = static_cast<uint8_t>(settings.value("EPL_DC/IH/prefetchSize").toInt());
  cfg.backConf.ihConfig.cleanupInterval = static_cast<uint8_t>(settings.value("EPL_DC/IH/cleanupInterval").toInt());
  cfg.backConf.ihConfig.checkPrefetch   = static_cast<uint8_t>(settings.value("EPL_DC/IH/checkPrefetch").toInt());
  cfg.backConf.ihConfig.deleteCyclesAfter =
        std::chrono::milliseconds(settings.value("EPL_DC/IH/deleteCyclesAfter").toInt());
  cfg.backConf.ihConfig.loopWaitTimeout =
        std::chrono::milliseconds(settings.value("EPL_DC/IH/loopWaitTimeout").toInt());

  std::vector<std::pair<QString, QColor *>> colorSelctor = {
        {"COL_odHighlight", &cfg.odHighlight},
        {"COL_PASnd", &cfg.pASnd},
        {"COL_pInvalid", &cfg.pInvalid},
        {"COL_pPReq", &cfg.pPReq},
        {"COL_pPRes", &cfg.pPRes},
        {"COL_pSoA", &cfg.pSoA},
        {"COL_pSoC", &cfg.pSoC},
        {"COL_ANMI", &cfg.pANMI},
        {"COL_AINV", &cfg.pAINV},

        {"COL_ProtoError", &cfg.evProtoError},
        {"COL_Error", &cfg.evError},
        {"COL_Warning", &cfg.evWarning},
        {"COL_Info", &cfg.evInfo},
        {"COL_Debug", &cfg.evDebug},
        {"COL_PText", &cfg.evPText},

        {"COL_nHighlighted", &cfg.NMT_Highlighted},
        {"COL_NMTOFF", &cfg.NMT_OFF},
        {"COL_INITIALISING", &cfg.NMT_INITIALISING},
        {"COL_RESET_APPLICATION", &cfg.NMT_RESET_APPLICATION},
        {"COL_RESET_COMMUNICATION", &cfg.NMT_RESET_COMMUNICATION},
        {"COL_RESET_CONFIGURATION", &cfg.NMT_RESET_CONFIGURATION},
        {"COL_NOT_ACTIVE", &cfg.NMT_NOT_ACTIVE},
        {"COL_PRE_OPERATIONAL_1", &cfg.NMT_PRE_OPERATIONAL_1},
        {"COL_PRE_OPERATIONAL_2", &cfg.NMT_PRE_OPERATIONAL_2},
        {"COL_READY_TO_OPERATE", &cfg.NMT_READY_TO_OPERATE},
        {"COL_OPERATIONAL", &cfg.NMT_OPERATIONAL},
        {"COL_STOPPED", &cfg.NMT_STOPPED},
        {"COL_BASIC_ETHERNET", &cfg.NMT_BASIC_ETHERNET},
  };

  for (auto i : colorSelctor) {
    i.second->setNamedColor(settings.value(i.first).toString());
  }

  cfg.nodes.clear();
  cfg.plots.clear();

  int size = settings.beginReadArray("nodes");
  for (int i = 0; i < size; ++i) {
    settings.setArrayIndex(i);
    int         index                          = settings.value("id").toInt();
    bool        autoDed                        = settings.value("autoDeduceSpecificProfile").toBool();
    std::string base                           = settings.value("baseProfile").toString().toStdString();
    std::string specificProfile                = settings.value("specificProfile").toString().toStdString();
    cfg.nodes[index].autoDeduceSpecificProfile = autoDed;
    cfg.nodes[index].baseProfile               = base;
    cfg.nodes[index].specificProfile           = specificProfile;
  }
  settings.endArray();

  size = settings.beginReadArray("plots");
  for (int i = 0; i < size; ++i) {
    settings.setArrayIndex(i);
    cfg.plots.emplace_back();
    auto &plt = cfg.plots.back();

    plt.isOK          = true;
    plt.defaultODPlot = settings.value("defaultODPlot").toBool();
    plt.node          = static_cast<uint8_t>(settings.value("node").toInt());
    plt.index         = static_cast<uint16_t>(settings.value("index").toInt());
    plt.subIndex      = static_cast<uint8_t>(settings.value("subIndex").toInt());
    plt.addToTimeLine = settings.value("addToTimeLine").toBool();
    plt.addToPlot     = settings.value("addToPlot").toBool();
    plt.csName        = settings.value("csName").toString().toStdString();
    plt.color.setNamedColor(settings.value("color").toString());
  }
  settings.endArray();

  return true;
}
