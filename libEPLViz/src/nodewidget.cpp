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
 */

#include "nodewidget.hpp"

NodeWidget::NodeWidget(EPL_DataCollect::Node *node, QWidget *parent) : QStackedWidget(parent) {
  id       = node->getID();
  idString = QString::number(id);

  auto identity = node->getIdentity();

  name   = QString::fromStdString(identity.HostName);
  device = QString::number(identity.Profile);
  status = node->getStatus();


  setObjectName(QStringLiteral("node") + idString);

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
  line->setObjectName(QStringLiteral("line") + idString);
  line->setFrameShadow(QFrame::Plain);
  line->setFrameShape(QFrame::HLine);

  // Create name label
  nameLabel = new QLabel(this);
  nameLabel->setObjectName(QStringLiteral("name") + idString);
  nameLabel->setText(nameFormat.arg(name, idString));

  // Create status label
  statusLabel = new QLabel(this);
  statusLabel->setObjectName(QStringLiteral("status") + idString);
  statusLabel->setObjectName(statusFormat.arg(QString::fromStdString(EPL_DataCollect::EPLEnum2Str::toStr(status))));

  // Create advanced information tree
  advancedInfo = new QTreeWidget(this);
  advancedInfo->setObjectName(QStringLiteral("advancedInfo") + idString);
  advancedInfo->setEnabled(true);
  advancedInfo->setAutoFillBackground(false);
  advancedInfo->setStyleSheet(QStringLiteral(""));
  advancedInfo->setVisible(false);        // Hide it until the user checks 'advanced'
  advancedInfo->setSortingEnabled(false); // Do not sort values

  // Fill the advanced information tree with values
  QTreeWidgetItem *dev = new QTreeWidgetItem(advancedInfo);
  dev->setText(0, QString("Device"));

  QTreeWidgetItem *net = new QTreeWidgetItem(advancedInfo);
  net->setText(0, QString("Network"));

  // Device information
  QTreeWidgetItem *vid = new QTreeWidgetItem(dev);
  vid->setText(0, QString("Vendor ID"));

  QTreeWidgetItem *prdc = new QTreeWidgetItem(dev);
  prdc->setText(0, QString("Product Code"));

  QTreeWidgetItem *sno = new QTreeWidgetItem(dev);
  sno->setText(0, QString("Serial Number"));

  QTreeWidgetItem *rno = new QTreeWidgetItem(dev);
  rno->setText(0, QString("Revision Number"));

  // Networking information
  QTreeWidgetItem *ipa = new QTreeWidgetItem(net);
  ipa->setText(0, QString("IP Address"));

  QTreeWidgetItem *snm = new QTreeWidgetItem(net);
  snm->setText(0, QString("Subnet Mask"));

  QTreeWidgetItem *gtw = new QTreeWidgetItem(net);
  gtw->setText(0, QString("Default Gateway"));

  // Update the information
  updateAdvancedInfo(identity);

  // Create type label
  typeLabel = new QLabel(this);
  typeLabel->setObjectName(QStringLiteral("type") + idString);
  typeLabel->setStyleSheet(QStringLiteral(""));

  // Create the minimize/maximize button
  minimizeButton = new QPushButton(this);
  minimizeButton->setObjectName(QStringLiteral("minimizeButton") + idString);
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
  advanced->setObjectName(QStringLiteral("advanced") + idString);
  advanced->setStyleSheet(QStringLiteral(""));
  advanced->setChecked(true);

  // Create the layout for the maximized widget
  QGridLayout *maximizedLayout = new QGridLayout(this);
  maximizedLayout->setSpacing(6);
  maximizedLayout->setContentsMargins(11, 11, 11, 11);
  maximizedLayout->setObjectName(QStringLiteral("maximizedLayout") + idString);

  // Add all widgets to the maximized layout
  maximizedLayout->addWidget(line, 2, 0, 1, 3);
  maximizedLayout->addWidget(nameLabel, 0, 0, 1, 1, Qt::AlignLeft | Qt::AlignTop);
  maximizedLayout->addWidget(statusLabel, 1, 0, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);
  maximizedLayout->addWidget(advancedInfo, 5, 0, 1, 3);
  maximizedLayout->addWidget(typeLabel, 1, 2, 1, 1, Qt::AlignRight | Qt::AlignVCenter);
  maximizedLayout->addWidget(minimizeButton, 0, 2, 1, 1, Qt::AlignRight | Qt::AlignTop);
  maximizedLayout->addWidget(advanced, 4, 0, 1, 3);

  // Add the layout to the maximized Widget
  maxWidget->setLayout(maximizedLayout);

  // Create the layout for the minimized layout
  QVBoxLayout *minimizedLayout = new QVBoxLayout(this);
  minimizedLayout->setSpacing(6);
  minimizedLayout->setObjectName(QStringLiteral("minimizedLayout") + idString);
  minimizedLayout->setContentsMargins(2, 2, 2, 2);

  // Add members of the minimized layout
  minimizedLayout->addWidget(minimizeButton, 0, Qt::AlignRight | Qt::AlignTop);
  minimizedLayout->addWidget(nameLabel, 0, Qt::AlignHCenter | Qt::AlignBottom);

  // Add the layout to the maximized Widget
  minWidget->setLayout(minimizedLayout);

  // Connect the signals of the buttons
  connect(minimizeButton, SIGNAL(toggled(bool)), this, SLOT(minimizeChange(bool)));
  connect(advanced, SIGNAL(toggled(bool)), advancedInfo, SLOT(setVisible(bool)));

  // Add the two states of the widget to this one
  addWidget(maxWidget);
  addWidget(minWidget);
  widget(1)->show(); // Show the minimized widget
}

/*
* \brief Turns a NMTState into its respective rgb color value.
* \param _status The status to retrieve the color of
* \return The rgb color value for the given status
*/
QString NodeWidget::statusToBackground(EPL_DataCollect::NMTState _status) {
  switch (_status) {
    case EPL_DataCollect::NMTState::OFF: return QString("rgb(158, 158, 158)");
    case EPL_DataCollect::NMTState::INITIALISING:
    case EPL_DataCollect::NMTState::RESET_APPLICATION:
    case EPL_DataCollect::NMTState::RESET_COMMUNICATION:
    case EPL_DataCollect::NMTState::RESET_CONFIGURATION:
    case EPL_DataCollect::NMTState::NOT_ACTIVE:
    case EPL_DataCollect::NMTState::PRE_OPERATIONAL_1:
    case EPL_DataCollect::NMTState::PRE_OPERATIONAL_2: return QString("rgb(255, 193, 7)");
    case EPL_DataCollect::NMTState::READY_TO_OPERATE: return QString("rgb(33, 150, 243)");
    case EPL_DataCollect::NMTState::OPERATIONAL: return QString("rgb(139, 195, 74)");
    case EPL_DataCollect::NMTState::STOPPED: return QString("rgb(244, 67, 54)");
    case EPL_DataCollect::NMTState::BASIC_ETHERNET: return QString("rgb(156, 39, 176)");
  }
  return QString("rgb(0, 0, 0)");
}

/*!
 * \brief Updates the data of this node widget.
 * \param node The pointer to the node model
 */
void NodeWidget::updateData(EPL_DataCollect::Node *node) {
  updateAdvancedInfo(node->getIdentity());
  updateStatus(node->getStatus());
}

/*!
 * \brief Updates the data shown under the 'advanced' section.
 * \param identity The node identity to retrieve the information from
 */
void NodeWidget::updateAdvancedInfo(EPL_DataCollect::Node::IDENT identity) {
  QTreeWidgetItem *dev = advancedInfo->topLevelItem(0);
  QTreeWidgetItem *net = advancedInfo->topLevelItem(1);

  // Update Device information
  dev->child(0)->setText(1, QString::number(identity.VendorId));
  dev->child(1)->setText(1, QString::number(identity.ProductCode));
  dev->child(2)->setText(1, QString::number(identity.SerialNumber));
  dev->child(3)->setText(1, QString::number(identity.RevisionNumber));

  // Update Networking information
  net->child(0)->setText(1, QString::fromStdString(identity.IPAddress));
  net->child(1)->setText(1, QString::fromStdString(identity.SubnetMask));
  net->child(2)->setText(1, QString::fromStdString(identity.DefaultGateway));
}

/*!
 * \brief Updates the status of this node widget.
 * \param newStatus The new node status
 */
void NodeWidget::updateStatus(EPL_DataCollect::NMTState newStatus) {
  if (status == newStatus)
    return;

  // Update status
  status = newStatus;

  setStyleSheet(style.arg(idString, statusToBackground(status))); // Change color
  statusLabel->setObjectName(
        statusFormat.arg(QString::fromStdString(EPL_DataCollect::EPLEnum2Str::toStr(status)))); // Update label
}

void NodeWidget::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    emit nodeChanged(id);
  }
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
