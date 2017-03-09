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
 * \file NodeWidget.cpp
 */


#include "NodeWidget.hpp"

NodeWidget::NodeWidget(EPL_DataCollect::Node *node, QWidget *parent) : QStackedWidget(parent) {
  id       = node->getID();
  idString = QString::number(id);

  auto identity = node->getIdentity();

  connect(this, SIGNAL(nodeChanged(uint8_t)), parent, SLOT(selectNode(uint8_t)));
  setObjectName(QStringLiteral("node") + idString);

  // Create widget for minimized view
  minWidget = new QWidget(this);
  minWidget->setObjectName(QStringLiteral("node-min") + idString);

  // Create widget for maximized view
  maxWidget = new QWidget(this);
  maxWidget->setObjectName(QStringLiteral("node-max") + idString);

  // Create seperator line
  line = new QFrame(maxWidget);
  line->setObjectName(QStringLiteral("line") + idString);
  line->setFrameShadow(QFrame::Plain);
  line->setFrameShape(QFrame::HLine);

  // Create status label
  statusLabel = new QLabel(maxWidget);
  statusLabel->setObjectName(QStringLiteral("status") + idString);
  statusLabel->setObjectName(statusFormat.arg(QString::fromStdString(EPL_DataCollect::EPLEnum2Str::toStr(status))));

  // Create advanced information tree
  advancedInfo = new QTreeWidget(maxWidget);
  advancedInfo->setObjectName(QStringLiteral("advancedInfo") + idString);
  advancedInfo->setEnabled(true);
  advancedInfo->setAutoFillBackground(false);
  advancedInfo->setStyleSheet(QStringLiteral(""));
  advancedInfo->setVisible(false);        // Hide it until the user checks 'advanced'
  advancedInfo->setSortingEnabled(false); // Do not sort values
  advancedInfo->setHeaderLabels(QStringList() << "Name"
                                              << "Value");

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

  // Create name label for the maximized version
  nameLabelMax = new QLabel(maxWidget);
  nameLabelMax->setObjectName(QStringLiteral("name-max") + idString);

  // Create name label for the minimized version
  nameLabelMin = new QLabel(minWidget);
  nameLabelMin->setObjectName(QStringLiteral("name-min") + idString);

  // Create type label
  typeLabel = new QLabel(maxWidget);
  typeLabel->setObjectName(QStringLiteral("type") + idString);
  typeLabel->setStyleSheet(QStringLiteral(""));

  // Update the information
  updateIdentityInfo(identity);

  // Add icons for maximizing (Box is checked) and minimizing (Box is unchecked)
  QIcon minmaxIcon;
  minmaxIcon.addFile(QStringLiteral(":/icons/resources/minimize.png"), QSize(), QIcon::Normal, QIcon::Off);
  minmaxIcon.addFile(QStringLiteral(":/icons/resources/maximize.png"), QSize(), QIcon::Normal, QIcon::On);

  // Create the maximize button
  maximizeButton = new QPushButton(maxWidget);
  maximizeButton->setObjectName(QStringLiteral("maximizeButton") + idString);
  maximizeButton->setMinimumSize(QSize(13, 13));
  maximizeButton->setMaximumSize(QSize(13, 13));
  maximizeButton->setStyleSheet(QStringLiteral(""));
  maximizeButton->setIcon(minmaxIcon);
  maximizeButton->setIconSize(QSize(13, 13));
  maximizeButton->setCheckable(true);
  maximizeButton->setChecked(false);

  // Create the minimize button
  minimizeButton = new QPushButton(minWidget);
  minimizeButton->setObjectName(QStringLiteral("minimizeButton") + idString);
  minimizeButton->setMinimumSize(QSize(13, 13));
  minimizeButton->setMaximumSize(QSize(13, 13));
  minimizeButton->setStyleSheet(QStringLiteral(""));
  minimizeButton->setIcon(minmaxIcon);
  minimizeButton->setIconSize(QSize(13, 13));
  minimizeButton->setCheckable(true);
  minimizeButton->setChecked(false);

  // Create the advanced button
  advanced = new QRadioButton(maxWidget);
  advanced->setObjectName(QStringLiteral("advanced") + idString);
  advanced->setText("Show advanced information...");
  advanced->setStyleSheet(QStringLiteral(""));
  advanced->setChecked(false);

  // Create the layout for the maximized widget
  QGridLayout *maximizedLayout = new QGridLayout(maxWidget);
  maximizedLayout->setSpacing(6);
  maximizedLayout->setContentsMargins(11, 11, 11, 11);
  maximizedLayout->setObjectName(QStringLiteral("maximizedLayout") + idString);

  // Add all widgets to the maximized layout
  maximizedLayout->addWidget(line, 2, 0, 1, 3);
  maximizedLayout->addWidget(nameLabelMax, 0, 0, 1, 1, Qt::AlignLeft | Qt::AlignTop);
  maximizedLayout->addWidget(statusLabel, 1, 0, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);
  maximizedLayout->addWidget(advancedInfo, 5, 0, 1, 3);
  maximizedLayout->addWidget(typeLabel, 1, 2, 1, 1, Qt::AlignRight | Qt::AlignVCenter);
  maximizedLayout->addWidget(minimizeButton, 0, 2, 1, 1, Qt::AlignRight | Qt::AlignTop);
  maximizedLayout->addWidget(advanced, 4, 0, 1, 3);

  maximizedLayout->setSizeConstraint(QLayout::SetFixedSize);

  // Add the layout to the maximized Widget
  maxWidget->setLayout(maximizedLayout);

  // Create the layout for the minimized layout
  QVBoxLayout *minimizedLayout = new QVBoxLayout(minWidget);
  minimizedLayout->setSpacing(6);
  minimizedLayout->setObjectName(QStringLiteral("minimizedLayout") + idString);
  minimizedLayout->setContentsMargins(2, 2, 2, 2);

  minimizedLayout->setSizeConstraint(QLayout::SetFixedSize);


  // Add members of the minimized layout
  minimizedLayout->addWidget(maximizeButton, 0, Qt::AlignRight | Qt::AlignTop);
  minimizedLayout->addWidget(nameLabelMin, 0, Qt::AlignHCenter | Qt::AlignBottom);

  // Add the layout to the minimized Widget
  minWidget->setLayout(minimizedLayout);

  // Connect the signals of the buttons
  connect(minimizeButton, SIGNAL(toggled(bool)), this, SLOT(minimizeChange(bool)));
  connect(maximizeButton, SIGNAL(toggled(bool)), this, SLOT(minimizeChange(bool)));
  connect(advanced, SIGNAL(toggled(bool)), advancedInfo, SLOT(setVisible(bool)));
  connect(advancedInfo, SIGNAL(itemExpanded(QTreeWidgetItem *)), this, SLOT(fixTree(QTreeWidgetItem *)));
  connect(advancedInfo, SIGNAL(itemCollapsed(QTreeWidgetItem *)), this, SLOT(fixTree(QTreeWidgetItem *)));


  // Add the two states of the widget to this one
  addWidget(maxWidget);
  addWidget(minWidget);

  updateStatus(node->getStatus());

  widget(1)->hide();
  widget(0)->show(); // Show the maximized widget when starting
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
  // Check if the node is the correct one
  if (node->getID() == id) {
    updateIdentityInfo(node->getIdentity());
    updateStatus(node->getStatus());
  }
}

void NodeWidget::setHighlightingLevel(int level) { highlightingLevel = level; }

void NodeWidget::updateStyleSheet() {
  QString statusColor = statusToBackground(status);
  QString highlight   = QString::number(static_cast<int>(255 * (static_cast<double>(highlightingLevel) / 100)));

  maxWidget->setStyleSheet(styleFormat.arg("max", idString, statusColor, highlight));
  minWidget->setStyleSheet(styleFormat.arg("min", idString, statusColor, highlight));
}

/*!
 * \brief Updates the data given by a nodes identity.
 * \param identity The node identity to retrieve the information from
 */
void NodeWidget::updateIdentityInfo(EPL_DataCollect::Node::IDENT identity) {
  QString name = QString::fromStdString(identity.HostName);

  // Update name
  nameLabelMin->setText(nameFormat.arg(name, idString));
  nameLabelMax->setText(nameFormat.arg(name, idString));

  // Update type
  typeLabel->setText(validateUInt(identity.Profile));

  QTreeWidgetItem *dev = advancedInfo->topLevelItem(0);
  QTreeWidgetItem *net = advancedInfo->topLevelItem(1);

  // Update Device information
  dev->child(0)->setText(1, validateUInt(identity.VendorId));
  dev->child(1)->setText(1, validateUInt(identity.ProductCode));
  dev->child(2)->setText(1, validateUInt(identity.SerialNumber));
  dev->child(3)->setText(1, validateUInt(identity.RevisionNumber));

  // Update Networking information
  net->child(0)->setText(1, QString::fromStdString(identity.IPAddress));
  net->child(1)->setText(1, QString::fromStdString(identity.SubnetMask));
  net->child(2)->setText(1, QString::fromStdString(identity.DefaultGateway));
}

/*!
 * \brief Checks if the given value is a valid value for the uint size.
 * \param val The uint16_t to validate
 * \return A string containing either the value, if valid, or '<N/A>'
 */
QString NodeWidget::validateUInt(uint16_t val) {
  if (val == UINT16_MAX)
    return "<N/A>";
  else
    return QString::number(val);
}

/*!
 * \brief Checks if the given value is a valid value for the uint size.
 * \param val The uint32_t to validate
 * \return A string containing either the value, if valid, or '<N/A>'
 */
QString NodeWidget::validateUInt(uint32_t val) {
  if (val == UINT32_MAX)
    return "<N/A>";
  else
    return QString::number(val);
}

void NodeWidget::fixTree(QTreeWidgetItem *item) {
  int column = advancedInfo->indexOfTopLevelItem(item);
  advancedInfo->resizeColumnToContents(column);
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

  statusLabel->setText(
        statusFormat.arg(QString::fromStdString(EPL_DataCollect::EPLEnum2Str::toStr(status)))); // Update label

  updateStyleSheet(); // TODO: Remove this
}

void NodeWidget::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    qDebug() << "Clicked the node " << idString;
    emit nodeChanged(id);
  }
}

void NodeWidget::minimizeChange(bool minimized) {
  qDebug() << "Minimize changed to " << minimized << " from " << isMinimized;
  // Prevent unnecessary changes
  if (isMinimized == minimized)
    return;

  // Call the correct function for the size change
  if (minimized) {
    widget(0)->hide();
    widget(1)->show();
  } else {
    widget(1)->hide();
    widget(0)->show();
  }

  // Update minimize value
  isMinimized = minimized;

  // Synchronize buttons
  minimizeButton->setChecked(isMinimized);
  maximizeButton->setChecked(isMinimized);
}
