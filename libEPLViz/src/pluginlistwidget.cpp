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
 * \file pluginlistwidget.cpp
 */

#include "pluginlistwidget.hpp"

PluginListWidget::PluginListWidget(QWidget *parent) : QListWidget(parent) {}

void PluginListWidget::selectedChanged(QListWidgetItem *current, QListWidgetItem *previous) {
  (void)previous;

  QString name = current->text();

  // Determine the modified state of the newly selected plugin
  if (name.endsWith('*')) {
    name.chop(1);
    modified = true;
  } else {
    modified = false;
  }

  emit pluginSelected(name);
}

bool PluginListWidget::isListed(QString file) {
  for (auto i = 0; i < count(); i++) {
    // Check if the file paths match
    if (item(i)->toolTip() == file)
      return true;
  }
  return false;
}

void PluginListWidget::newFile() {
  // Create new entry
  QListWidgetItem *item = new QListWidgetItem();

  if (newCounter > 1)
    item->setText(QString("Untitled (%1)").arg(newCounter));
  else
    item->setText(QString("Untitled"));
  item->setToolTip("Not saved yet!");
  item->setStatusTip("Not saved yet!");

  // Make text italic for highlighting
  QFont font = item->font();
  font.setItalic(true);
  item->setFont(font);

  this->addItem(item);
  this->setCurrentItem(item);

  newCounter++;
}

void PluginListWidget::fileAdded(QString file) {
  // Check that the file is not already opened
  if (isListed(file))
    return;

  // Deselect current item
  if (currentItem())
    currentItem()->setSelected(false);

  // Create a QFileInfo to extract name and path of file
  QFileInfo fileInfo(file);

  // Create new entry
  QListWidgetItem *item = new QListWidgetItem();
  item->setText(fileInfo.fileName());
  item->setSelected(true);
  item->setToolTip(fileInfo.filePath());
  item->setStatusTip(fileInfo.filePath());
  this->addItem(item);

  emit fileOpened(file);
}

void PluginListWidget::fileModified(bool newState) {
  if (modified != newState) {
    QString name = currentItem()->text();
    QFont   font = currentItem()->font();

    if (newState) {
      name.append('*');
      font.setBold(true);
    } else {
      name.chop(1);
      font.setBold(false);
    }

    currentItem()->setText(name);
    currentItem()->setFont(font);
    modified = newState;
  }
}

void PluginListWidget::nameChanged(QString fileName) {
  // Reset italic font on saved entries
  if (!currentItem())
    return;

  if (currentItem()->text().startsWith("Untitled")) {
    QFont font = currentItem()->font();
    font.setItalic(false);
    currentItem()->setFont(font);
  }

  currentItem()->setText(fileName);
}

void PluginListWidget::urlChanged(QString path) {
  if (!currentItem())
    return;

  currentItem()->setToolTip(path);
  currentItem()->setStatusTip(path);
}
