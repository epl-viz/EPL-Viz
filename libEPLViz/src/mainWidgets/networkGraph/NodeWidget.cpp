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
#include "MainWindow.hpp"

using namespace EPL_Viz;

NodeWidget::NodeWidget(MainWindow *mainWin, EPL_DataCollect::Node *node, QWidget *parent) : QFrame(parent) {
  mw       = mainWin;
  id       = node->getID();
  idString = QString::number(id);

  auto identity = node->getIdentity();

  setObjectName(QStringLiteral("node") + idString);

  // Create seperator line
  line = new QFrame(this);
  line->setObjectName(QStringLiteral("line") + idString);
  line->setFrameShadow(QFrame::Plain);
  line->setFrameShape(QFrame::HLine);

  // Create name label
  nameLabel = new QLabel(this);
  nameLabel->setObjectName(QStringLiteral("name") + idString);
  nameLabel->setToolTip(QString("The host name of this node and its node ID."));

  // Create type label
  typeLabel = new QLabel(this);
  typeLabel->setObjectName(QStringLiteral("type") + idString);
  typeLabel->setStyleSheet(QStringLiteral(""));
  typeLabel->setToolTip(QString("The device profile used by this node."));

  // Create status label
  statusLabel = new QLabel(this);
  statusLabel->setObjectName(QStringLiteral("status") + idString);
  statusLabel->setToolTip("Shows the current state machine status of the node.");

  // Create advanced information tree
  advancedInfo = new QTreeWidget(this);
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

  // Update the information
  updateIdentityInfo(identity);

  // Create the advanced button
  advanced = new QCheckBox(this);
  advanced->setObjectName(QStringLiteral("advanced") + idString);
  advanced->setText("Show advanced information...");
  advanced->setStyleSheet(QStringLiteral(""));
  advanced->setChecked(false);
  advanced->setToolTip("Provides additional information about the node.");

  // Create the layout for the maximized widget
  QGridLayout *gridLayout = new QGridLayout(this);
  gridLayout->setSpacing(6);
  gridLayout->setContentsMargins(11, 11, 11, 11);
  gridLayout->setObjectName(QStringLiteral("gridLayout") + idString);

  // Add all widgets to the maximized layout
  gridLayout->addWidget(line, 2, 0, 1, 3);
  gridLayout->addWidget(nameLabel, 0, 0, 1, 1, Qt::AlignLeft | Qt::AlignTop);
  gridLayout->addWidget(statusLabel, 1, 0, 1, 1, Qt::AlignLeft | Qt::AlignVCenter);
  gridLayout->addWidget(advancedInfo, 5, 0, 1, 3);
  gridLayout->addWidget(typeLabel, 1, 2, 1, 1, Qt::AlignRight | Qt::AlignVCenter);
  gridLayout->addWidget(advanced, 4, 0, 1, 3);

  gridLayout->setSizeConstraint(QLayout::SetFixedSize);

  // Add the layout to the maximized Widget
  setLayout(gridLayout);

  // Connect the signals of the buttons
  connect(advanced, SIGNAL(toggled(bool)), advancedInfo, SLOT(setVisible(bool)));

  updateStatus(node->getStatus());
}

/*
* \brief Turns a NMTState into its respective rgb color value.
* \param _status The status to retrieve the color of
* \return The rgb color value for the given status
*/
QColor NodeWidget::statusToBackground(EPL_DataCollect::NMTState _status) {
  switch (_status) {
    case EPL_DataCollect::NMTState::OFF: return mw->getSettingsWin()->getConfig().NMT_OFF;
    case EPL_DataCollect::NMTState::INITIALISING: return mw->getSettingsWin()->getConfig().NMT_INITIALISING;
    case EPL_DataCollect::NMTState::RESET_APPLICATION: return mw->getSettingsWin()->getConfig().NMT_RESET_APPLICATION;
    case EPL_DataCollect::NMTState::RESET_COMMUNICATION:
      return mw->getSettingsWin()->getConfig().NMT_RESET_COMMUNICATION;
    case EPL_DataCollect::NMTState::RESET_CONFIGURATION:
      return mw->getSettingsWin()->getConfig().NMT_RESET_CONFIGURATION;
    case EPL_DataCollect::NMTState::NOT_ACTIVE: return mw->getSettingsWin()->getConfig().NMT_NOT_ACTIVE;
    case EPL_DataCollect::NMTState::PRE_OPERATIONAL_1: return mw->getSettingsWin()->getConfig().NMT_PRE_OPERATIONAL_1;
    case EPL_DataCollect::NMTState::PRE_OPERATIONAL_2: return mw->getSettingsWin()->getConfig().NMT_PRE_OPERATIONAL_2;
    case EPL_DataCollect::NMTState::READY_TO_OPERATE: return mw->getSettingsWin()->getConfig().NMT_READY_TO_OPERATE;
    case EPL_DataCollect::NMTState::OPERATIONAL: return mw->getSettingsWin()->getConfig().NMT_OPERATIONAL;
    case EPL_DataCollect::NMTState::STOPPED: return mw->getSettingsWin()->getConfig().NMT_STOPPED;
    case EPL_DataCollect::NMTState::BASIC_ETHERNET: return mw->getSettingsWin()->getConfig().NMT_BASIC_ETHERNET;
  }
  return QString("rgb(0, 0, 0)");
}

/*!
 * \brief Returns the currently required border style
 *
 * If the node is selected, then display it with a 'double' border, otherwise use 'outset' to show highlighting.
 *
 * \return The string representing the type of border currently required
 */
QString NodeWidget::borderStyle() {
  if (selected)
    return "solid";
  else
    return "dashed";
}

/*!
 * \brief Updates the data of this node widget.
 * \param nID The node ID of the node
 * \param c   A reference to the cycle container
 */
void NodeWidget::updateData(uint8_t nID, ProtectedCycle &c) {
  auto                   lock = c.getLock();
  EPL_DataCollect::Node *node = c->getNode(nID);

  updateIdentityInfo(node->getIdentity());
  updateStatus(node->getStatus());
}

void NodeWidget::setHighlighted(bool highlight) { highlighted = highlight; }

void NodeWidget::setSelected(bool sel) {
  if (sel != selected) {
    selected = sel;
    updateStyleSheet();
  }
}

bool NodeWidget::isSelected() { return selected; }



void NodeWidget::updateStyleSheet() {
  QColor  statusColor = statusToBackground(status);
  QString highlight   = (highlighted ? mw->getSettingsWin()->getConfig().NMT_Highligthed.name() : "#000000");
  QString textColor   = statusColor.lightness() >= 125 ? "#000000" : "#ffffff";

  setStyleSheet(styleFormat.arg(idString, statusColor.name(), textColor, highlight, borderStyle()));
}

/*!
 * \brief Updates the data given by a nodes identity.
 * \param identity The node identity to retrieve the information from
 */
void NodeWidget::updateIdentityInfo(EPL_DataCollect::Node::IDENT identity) {
  QString name = validateString(identity.HostName);
  QString type = validateUInt(identity.Profile);

  // Update name
  nameLabel->setText(nameFormat.arg(name, idString));

  // Update type
  typeLabel->setText(typeFormat.arg(type));

  QTreeWidgetItem *dev = advancedInfo->topLevelItem(0);
  QTreeWidgetItem *net = advancedInfo->topLevelItem(1);

  // Update Device information
  dev->child(0)->setText(1, validateUInt(identity.VendorId));
  dev->child(1)->setText(1, validateUInt(identity.ProductCode));
  dev->child(2)->setText(1, validateUInt(identity.SerialNumber));
  dev->child(3)->setText(1, validateUInt(identity.RevisionNumber));

  // Update Networking information
  net->child(0)->setText(1, validateString(identity.IPAddress));
  net->child(1)->setText(1, validateString(identity.SubnetMask));
  net->child(2)->setText(1, validateString(identity.DefaultGateway));
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

QString NodeWidget::validateString(std::string string) {
  if (string == "")
    return "<N/A>";
  else
    return QString::fromStdString(string);
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
}

void NodeWidget::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    qDebug() << "Clicked the node " << idString;
    emit nodeClicked(id);
  }
}
