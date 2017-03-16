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
 * \file InterfacePicker.cpp
 */

#include "InterfacePicker.hpp"
#include "MainWindow.hpp"
#include "ui_interfacepicker.h"
using namespace EPL_Viz;
using namespace EPL_DataCollect;

InterfacePicker::InterfacePicker(MainWindow *parent, CaptureInstance *ci)
    : QDialog(parent), ui(new Ui::InterfacePicker) {
  captureInstance = ci;
  ui->setupUi(this);
  mw = parent;
}

InterfacePicker::~InterfacePicker() { delete ui; }

bool InterfacePicker::event(QEvent *event) {
  // configure stuff
  if (event->type() == QEvent::Polish) {
    updateList();
  }
  return QDialog::event(event);
}

void InterfacePicker::updateList() {
  QListWidget *            list       = findChild<QListWidget *>("interfaceList");
  std::vector<std::string> interfaces = captureInstance->getDevices();
  for (std::string s : interfaces) {
    list->addItem(QString::fromStdString(s));
    qDebug() << "Added" + QString::fromStdString(s);
  }
}

QString InterfacePicker::getSelection() {
  QList<QListWidgetItem *> items = ui->interfaceList->selectedItems();
  if (items.isEmpty())
    return "";

  return items[0]->data(Qt::DisplayRole).toString();
}


QString InterfacePicker::getInterface(MainWindow *parent, CaptureInstance *ci) {
  InterfacePicker picker(parent, ci);
  int             res = picker.exec();
  if (res != QDialog::Accepted)
    return "";
  else
    return picker.getSelection();
}
