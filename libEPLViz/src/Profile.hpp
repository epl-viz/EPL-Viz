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
 * \file Profile.hpp
 */

#pragma once
#include <QSettings>
#include <QString>

namespace EPL_Viz {

class Profile {
 private:
  QSettings *settings;
  QString    name;
  QString    nameSaved;

 public:
  Profile(QSettings *settings, QString name);
  ~Profile() = default;

  QString getName();

  void writeInterface(QString interfaceSTR);
  /**
   * @brief readInterface Returns the saved Interface name or the QString "none"
   * @return A string
   */
  QString readInterface();

  void writeCustomValue(QString custom, QVariant val);
  /**
   * @brief readCustomValue Returns a QVariant with the saved value or an empty one
   * @param custom The new val
   * @return A Variant
   */
  QVariant readCustomValue(QString custom);

  void beginWriteArray(QString custom);
  int beginReadArray(QString custom);
  void setArrayIndex(int i);
  void endArray();
};
}
