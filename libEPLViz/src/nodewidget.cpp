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
 * \file nodewidget.cpp
 * \todo Implement signals/slots for the buttons
 */

#include "nodewidget.hpp"

NodeWidget::NodeWidget(uint8_t _id, QString _name, QString _device, QWidget *parent) : QStackedWidget(parent) {
  id     = _id;
  name   = _name;
  device = _device;
  setObjectName(QStringLiteral("node") + QString::number(_id));

  // Create widget for minimized view
  minWidget = new QWidget(this);
  minWidget->setMinimumSize(minSize);
  minWidget->setMaximumSize(minSize);

  // Create widget for maximized view
  maxWidget = new QWidget(this);
  maxWidget->setMinimumSize(maxSize);
  maxWidget->setMaximumSize(maxSize);

  // Create seperator line
  line = new QFrame(this);
  line->setObjectName(QStringLiteral("line") + QString::number(_id));
  line->setFrameShadow(QFrame::Plain);
  line->setFrameShape(QFrame::HLine);

  // Create name label
  nameLabel = new QLabel(this);
  nameLabel->setObjectName(QStringLiteral("name") + QString::number(_id));

  // Create status label
  statusLabel = new QLabel(this);
  statusLabel->setObjectName(QStringLiteral("status") + QString::number(_id));

  // Create ODList
  odList = new QTreeWidget(this);
  odList->setObjectName(QStringLiteral("odList") + QString::number(_id));
  odList->setEnabled(true);
  odList->setAutoFillBackground(false);
  odList->setStyleSheet(QStringLiteral(""));
  odList->setVisible(false); // Hide it until the user checks 'advanced'

  // Create type label
  typeLabel = new QLabel(this);
  typeLabel->setObjectName(QStringLiteral("type") + QString::number(_id));
  typeLabel->setStyleSheet(QStringLiteral(""));

  // Create the minimize/maximize button
  minimizeButton = new QPushButton(this);
  minimizeButton->setObjectName(QStringLiteral("minimizeButton") + QString::number(_id));
  minimizeButton->setMinimumSize(QSize(13, 13));
  minimizeButton->setMaximumSize(QSize(13, 13));
  minimizeButton->setStyleSheet(QStringLiteral(""));
  // Add icons for maximizing/minimizing
  QIcon minmaxIcon;
  minmaxIcon.addFile(QStringLiteral(":/icons/resources/minimize.png"), QSize(), QIcon::Normal, QIcon::Off);
  minmaxIcon.addFile(QStringLiteral(":/icons/resources/maximize.png"), QSize(), QIcon::Normal, QIcon::On);
  minimizeButton->setIcon(minmaxIcon);
  minimizeButton->setIconSize(QSize(13, 13));
  minimizeButton->setCheckable(true);
  minimizeButton->setChecked(true); // Set minimized as default

  // Create the advanced button
  advanced = new QRadioButton(this);
  advanced->setObjectName(QStringLiteral("advanced") + QString::number(_id));
  advanced->setStyleSheet(QStringLiteral(""));
  advanced->setChecked(true);

  // Create the layout for the maximized widget
  QGridLayout *maximizedLayout = new QGridLayout(this);
  maximizedLayout->setSpacing(6);
  maximizedLayout->setContentsMargins(11, 11, 11, 11);
  maximizedLayout->setObjectName(QStringLiteral("maximizedLayout") + QString::number(_id));

  // Add all widgets to the maximized layout
  maximizedLayout->addWidget(line, 2, 0, 1, 3);
  maximizedLayout->addWidget(nameLabel, 0, 0, 1, 1, Qt::AlignLeft | Qt::AlignTop);
  maximizedLayout->addWidget(statusLabel, 1, 0, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);
  maximizedLayout->addWidget(odList, 5, 0, 1, 3);
  maximizedLayout->addWidget(typeLabel, 1, 2, 1, 1, Qt::AlignRight | Qt::AlignVCenter);
  maximizedLayout->addWidget(minimizeButton, 0, 2, 1, 1, Qt::AlignRight | Qt::AlignTop);
  maximizedLayout->addWidget(advanced, 4, 0, 1, 3);

  // Add the layout to the maximized Widget
  maxWidget->setLayout(maximizedLayout);

  // Create the layout for the minimized layout
  QVBoxLayout *minimizedLayout = new QVBoxLayout(this);
  minimizedLayout->setSpacing(6);
  minimizedLayout->setObjectName(QStringLiteral("minimizedLayout") + QString::number(_id));
  minimizedLayout->setContentsMargins(2, 2, 2, 2);

  // Add members of the minimized layout
  minimizedLayout->addWidget(minimizeButton, 0, Qt::AlignRight | Qt::AlignTop);
  minimizedLayout->addWidget(nameLabel, 0, Qt::AlignHCenter | Qt::AlignBottom);

  // Add the layout to the maximized Widget
  minWidget->setLayout(minimizedLayout);


  addWidget(maxWidget);
  addWidget(minWidget);
  widget(1)->show(); // Show the minimized widget
}

/*
* \brief Turns a NodeStatus into its respective rgb color value.
* \param _status The status to retrieve the color of
* \returns The rgb color value for the given status
*/
QString NodeWidget::statusToBackground(EPL_DataCollect::NodeStatus _status) {
  switch (_status) {
    case EPL_DataCollect::NodeStatus::OK: return QString("rgb(139, 195, 74)");
    case EPL_DataCollect::NodeStatus::STARTING: return QString("rgb(33, 150, 243)");
    case EPL_DataCollect::NodeStatus::ERROR: return QString("rgb(244, 67, 54)");
    case EPL_DataCollect::NodeStatus::UNKNOWN: return QString("rgb(158, 158, 158)");
  }
  return QString("rgb(0, 0, 0)");
}

void NodeWidget::updateStatus(EPL_DataCollect::NodeStatus newStatus) {
  if (status == newStatus)
    return;

  // Update status
  status = newStatus;

  setStyleSheet(style.arg(QString::number(id), statusToBackground(status))); // Change color
  statusLabel->setObjectName(
        statusFormat.arg(QString::fromStdString(EPL_DataCollect::EPLEnum2Str::toStr(status)))); // Update label
}

void NodeWidget::minimizeChange(bool minimized) {
  // Prevent unnecessary changes
  if (isMinimized == minimized)
    return;

  // Call the correct function for the size change
  if (minimized)
    widget(1)->show();
  else
    widget(0)->show();

  // Update minimize value
  isMinimized = minimized;
}
