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

#include "HexSpinBox.hpp"

using namespace EPL_Viz;

HexSpinBox::HexSpinBox(QWidget *parent) : QSpinBox(parent) { setDisplayIntegerBase(16); }

QString HexSpinBox::textFromValue(int value) const { return QString::number(value, 16).toUpper(); }

int HexSpinBox::valueFromText(const QString &text) const { return static_cast<int>(text.toUInt(0, 16)); }

QValidator::State HexSpinBox::validate(QString &input, int &pos) const {
  QString copy(input);

  pos -= copy.size() - copy.trimmed().size();
  copy = copy.trimmed();
  if (copy.isEmpty())
    return QValidator::Intermediate;

  input = copy.toUpper();

  bool okay;
  int  val = static_cast<int>(copy.toUInt(&okay, 16));
  if (!okay || val < minimum() || val > maximum()) {
    return QValidator::Invalid;
  }

  return QValidator::Acceptable;
}
