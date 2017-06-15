/* Copyright (c) 2017, EPL-Vizards
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
 * \file DefaultPluginEditor.cpp
 */

#include "DefaultPluginEditor.hpp"

DefaultPluginEditor::DefaultPluginEditor(PluginEditorWidget *parent) : PluginEditorBase(parent) {
  layout = new QGridLayout(this);
}

DefaultPluginEditor::~DefaultPluginEditor() {}

void DefaultPluginEditor::createWidget() {
  if (!widget) {
    // The widget does not yet exist, create a new one
    widget = new QPlainTextEdit(this);
    layout->addWidget(widget);
    connect(widget->document(), SIGNAL(modificationChanged(bool)), this, SLOT(modified()));
  }

  widget->document()->setPlainText(contents[current]);

  // Check if the file was dirty
  if (!dirtyFiles.contains(current)) {
    widget->document()->setModified(false); // Setting the text of the editor modifies it
  }
}

void DefaultPluginEditor::updateStatusBar(bool enabled) { (void)enabled; }

void DefaultPluginEditor::openConfig() {
  QMessageBox::information(0, "Info", tr("There is currently no configuration for this editor"));
}

void DefaultPluginEditor::selectDocument(QString plugin) {
  if (widget) {
    // Check if the file is dirty
    if (widget->document()->isModified() && !dirtyFiles.contains(current)) {
      dirtyFiles.append(current);
      contents[current] = widget->document()->toPlainText(); // Also stash the changes
    }
  }
  current = plugin;
  createWidget();
}

void DefaultPluginEditor::closeDocument(QString name) {
  // Only try to close if the editor is open
  if (!widget)
    return;

  // Check if file is dirty
  if (dirtyFiles.contains(name) || (name == current && widget->document()->isModified())) {
    // Ask user for further input
    QMessageBox msg(this);

    QPushButton *saveButton = msg.addButton("Save", QMessageBox::ActionRole);
    msg.addButton("Discard", QMessageBox::ActionRole);


    msg.setText("The file '" + name + "' has unsaved changes.");
    msg.setInformativeText("Would you like to save them?");
    msg.setDefaultButton(saveButton);

    // Open message box
    msg.exec();

    QPushButton *clicked = dynamic_cast<QPushButton *>(msg.clickedButton());

    if (clicked == saveButton) {
      // Save the changes to this document
      save();
    } else {
      // Discard button was pressed, remove from dirty files
      dirtyFiles.removeOne(name);
    }
  }

  files.remove(name);
  contents.remove(name);

  current = QString();
  layout->removeWidget(widget);
  delete widget;
  widget = nullptr;
}

void DefaultPluginEditor::openDocument(QUrl file) {
  QFileInfo fileInfo(file.toLocalFile());
  QString   fileName;
  QString   filePath;

  if (fileInfo.exists() && fileInfo.isFile()) {
    fileName = fileInfo.fileName();
    filePath = fileInfo.filePath();

    files.insert(fileName, filePath);
  } else {
    QMessageBox::critical(0, "Error", tr("Could not open file '%1'.").arg(filePath));
    return;
  }

  QFile f(filePath);

  if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QMessageBox::critical(0, "Error", tr("Could not open file '%1'.").arg(filePath));
    return;
  }

  QTextStream in(&f);

  contents.insert(fileName, in.readAll());
  current = fileName;

  createWidget();
}

void DefaultPluginEditor::cleanUp() {
  QMessageBox  msg(this);
  QPushButton *saveButton       = msg.addButton("Save", QMessageBox::ActionRole);
  QPushButton *saveAllButton    = msg.addButton("Save All", QMessageBox::ActionRole);
  QPushButton *discardButton    = msg.addButton("Discard", QMessageBox::ActionRole);
  QPushButton *discardAllButton = msg.addButton("Discard All", QMessageBox::ActionRole);


  msg.setInformativeText("Would you like to save them?");
  msg.setDefaultButton(saveAllButton);

  bool saveAll = false;

  // Since dirty files are only acknowledged when "stashing them", check if the current file is dirty
  if (widget && !dirtyFiles.contains(current) && widget->document()->isModified()) {
    dirtyFiles.append(current);
  }

  QList<QString> list = dirtyFiles;

  for (QString file : list) {
    if (saveAll) {
      current = file;
      save();
      continue;
    }

    msg.setText("The file '" + file + "' has unsaved changes.");

    msg.exec();

    QPushButton *clicked = dynamic_cast<QPushButton *>(msg.clickedButton());

    if (clicked == saveButton) {
      // Save the changes to this document
      current = file;
      save();
      continue;
    } else if (clicked == saveAllButton) {
      // Save the changes to this document and remember the choice
      current = file;
      save();
      saveAll = true;
      continue;
    } else if (clicked == discardButton) {
      // Discard changes for current document
      continue;
    } else if (clicked == discardAllButton) {
      // Discard all changes
      break;
    }
  }

  QMap<QString, QString> savedFiles;

  // Destroy all documents and add them to the map of saved plugins
  for (QString file : files.keys()) {
    QString localFile = files[file];

    if (!dirtyFiles.contains(file)) {
      if (QFile::exists(localFile)) {
        savedFiles.insert(file, localFile);
      }
    }
  }


  emit pluginsSaved(savedFiles);
  files.clear();
  contents.clear();
  dirtyFiles.clear();
  current = QString();
  layout->removeWidget(widget);
  delete widget;
  widget     = nullptr;
  newCounter = 1;
}

void DefaultPluginEditor::writeToFile(QString filePath) {

  QFile file(filePath);

  contents[current] = widget->document()->toPlainText();

  // Try to open the file write-only, truncating any previous contents
  if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
    QTextStream out(&file);

    // Write the current editors content to the file
    out << contents[current] << "\n";

    file.close();
    emit modifiedChanged(false); // The file is now saved

    dirtyFiles.removeOne(current);
  } else {
    QMessageBox::critical(0, "Error", tr("Could not save to file '%1'.").arg(filePath));
  }
}

void DefaultPluginEditor::save() {
  // Abort when the editor is not open
  if (!widget)
    return;

  QString filePath = files[current];

  // If the file is not yet saved, save as instead
  if (filePath == QString()) {
    saveAs();
    return;
  }

  writeToFile(filePath);
}

void DefaultPluginEditor::saveAs() {
  // Abort when the editor is not open
  if (!widget)
    return;

  // Ask for a file to save to
  QString filePath =
        QFileDialog::getSaveFileName(this, tr("Save Document"), "", tr("Python files (*.py);;All Files (*)"));

  // Check if user selected a file
  if (filePath == QString::null)
    return;

  writeToFile(filePath);

  // Check if the name or path changed
  QFileInfo fileInfo(filePath);

  if (fileInfo.exists() && fileInfo.isFile()) {
    QString newName = fileInfo.fileName();

    // Check if the path changed
    if (filePath != files[current]) {

      // A name change can only occur when the path changes
      if (newName != current) {
        QString content = contents[current]; // Keep content

        files.remove(current); // Remove the entry as the key is invalid

        // Check if file was marked as dirty
        if (dirtyFiles.contains(current)) {
          dirtyFiles.removeOne(current);
        }

        current = newName;

        files.insert(current, filePath);
        contents.insert(current, content);
        nameChange();
      } else {
        files[current] = filePath; // Update the file path
      }

      urlChange();
    }
  }
}

void DefaultPluginEditor::newDocument() {
  if (newCounter > 1)
    current = QString("Untitled (%1)").arg(newCounter);
  else
    current = "Untitled";
  newCounter++;

  files.insert(current, QString());
  contents.insert(current, QString());

  createWidget();
  widget->document()->setModified(false);
}

void DefaultPluginEditor::modified() { emit modifiedChanged(widget->document()->isModified()); }

void DefaultPluginEditor::nameChange() { emit nameChanged(current); }

void DefaultPluginEditor::urlChange() { emit urlChanged(files[current]); }
