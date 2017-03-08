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
 * \file PluginEditorWidget.cpp
 */

#include "PluginEditorWidget.hpp"
#include <QAbstractButton>
#include <QPushButton>


PluginEditorWidget::PluginEditorWidget(QWidget *parent) : QWidget(parent) { layout = new QGridLayout(this); }

PluginEditorWidget::~PluginEditorWidget() { cleanUp(); }

void PluginEditorWidget::selectPlugin(QString plugin) {
  // Find the specified document for the given plugin
  for (auto d : KTextEditor::Editor::instance()->documents()) {
    if (d->documentName() == plugin) {
      doc = d;
      break;
    }
  }

  createWidget();
}

void PluginEditorWidget::loadDocument(QUrl fileName) {
  // Retrieve editor singleton
  KTextEditor::Editor *editor = KTextEditor::Editor::instance();

  // Create a new document
  doc = editor->createDocument(this);

  // Connect the signal of the editor buffer being changed to the modified() slot
  QObject::connect(doc, SIGNAL(modifiedChanged(KTextEditor::Document *)), this, SLOT(modified()));
  QObject::connect(doc, SIGNAL(documentNameChanged(KTextEditor::Document *)), this, SLOT(nameChange()));
  QObject::connect(doc, SIGNAL(documentUrlChanged(KTextEditor::Document *)), this, SLOT(urlChange()));



  // Load the file if specified
  if (fileName != QUrl()) {
    if (!doc->openUrl(fileName)) {
      qDebug() << "Failed to open " << fileName.toLocalFile();
      return;
    }
  }

  // Set syntax to python
  doc->setHighlightingMode("Python");
  emit doc->highlightingModeChanged(doc);

  createWidget();
}

void PluginEditorWidget::createWidget() {
  // Check if there is a widget already
  if (view) {
    // Remove the old widget
    layout->removeWidget(view);
    delete view;
  }

  // Create a widget to display the document
  view = doc->createView(this);

  // Set status bar
  view->setStatusBarEnabled(showStatusBar); // TODO: Add a variable for this to the global user profile

  // Add the widget to the layout
  layout->addWidget(view);
}

void PluginEditorWidget::modified() { emit modifiedChanged(doc->isModified()); }

void PluginEditorWidget::statusBarToggled(bool enabled) {
  if (view->isStatusBarEnabled() != enabled) {
    showStatusBar = enabled;

    // Update the widget if it exists
    if (view) {
      view->setStatusBarEnabled(enabled);
      emit view->statusBarEnabledChanged(view, enabled);
    }
  }
}
void PluginEditorWidget::configEditor() { KTextEditor::Editor::instance()->configDialog(this); }

void PluginEditorWidget::nameChange() { emit nameChanged(doc->documentName()); }

void PluginEditorWidget::urlChange() { emit urlChanged(doc->url().toString()); }

void PluginEditorWidget::save() {
  if (!doc)
    return;

  doc->documentSave();
}

void PluginEditorWidget::saveAs() {
  if (!doc)
    return;

  doc->documentSaveAs();
}

void PluginEditorWidget::newFile() { loadDocument(); }

void PluginEditorWidget::openFile(QUrl file) { loadDocument(file); }

void PluginEditorWidget::cleanUp() {
  QMessageBox  msg;
  QPushButton *saveButton       = msg.addButton("Save", QMessageBox::ActionRole);
  QPushButton *saveAllButton    = msg.addButton("Save All", QMessageBox::ActionRole);
  QPushButton *discardButton    = msg.addButton("Discard", QMessageBox::ActionRole);
  QPushButton *discardAllButton = msg.addButton("Discard All", QMessageBox::ActionRole);


  msg.setInformativeText("Would you like to save them?");
  msg.setDefaultButton(saveAllButton);

  bool saveAll = false;

  auto list = KTextEditor::Editor::instance()->documents();

  for (auto d : list) {
    if (d->isModified()) {

      if (saveAll) {
        d->documentSave();
        continue;
      }

      msg.setText("The plugin '" + d->documentName() + "' has unsaved changes.");

      msg.exec();

      QPushButton *clicked = dynamic_cast<QPushButton *>(msg.clickedButton());

      if (clicked == saveButton) {
        // Save the changes to this document
        d->documentSave();
        continue;
      } else if (clicked == saveAllButton) {
        // Save the changes to this document and remember the choice
        d->documentSave();
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
  }

  QMap<QString, QString> savedPlugins;

  // Destroy all documents and add them to the map of saved plugins
  for (auto d : list) {
    QString localFile = d->url().toLocalFile();
    qDebug() << localFile;

    if (QFile::exists(localFile)) {
      savedPlugins.insert(d->documentName(), localFile);
    }

    delete d;
  }

  emit pluginsSaved(savedPlugins);
  emit cleanupDone();
}
