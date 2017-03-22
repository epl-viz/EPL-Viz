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
 * \file Profile.cpp
 */

#include "Profile.hpp"
#include "MainWindow.hpp"
using namespace EPL_Viz;


Profile::Profile(QSettings *set, QString profileName) {
  settings = set;
  name     = profileName + "/";
}

QString Profile::getName() { return name; }

void Profile::writeWindowSettings(MainWindow *window) {
  settings->beginGroup(name + "MainWindow");
  settings->setValue("size", window->size());
  settings->setValue("pos", window->pos());
  settings->endGroup();
}

void Profile::readWindowSettings(MainWindow *window) {
  settings->beginGroup(name + "MainWindow");
  window->resize(settings->value("size", QSize(400, 400)).toSize());
  window->move(settings->value("pos", QPoint(200, 200)).toPoint());
  settings->endGroup();
}

void Profile::writeInterface(QString interface) { settings->setValue(name + "interface", interface); }

QString Profile::readInterface() { return settings->value(name + "interface", QVariant("none")).toString(); }

void Profile::writeCustomValue(QString custom, QVariant val) { settings->setValue(name + custom, val); }

QVariant Profile::readCustomValue(QString custom) { return settings->value(name + custom, QVariant()); }

void Profile::beginWriteArray(QString custom) {
  nameSaved = name;
  name      = "";
  settings->beginWriteArray(nameSaved + custom);
}

int Profile::beginReadArray(QString custom) {
  nameSaved = name;
  name      = "";
  return settings->beginReadArray(nameSaved + custom);
}

void Profile::setArrayIndex(int i) { settings->setArrayIndex(i); }

void Profile::endArray() {
  settings->endArray();
  name = nameSaved;
}
