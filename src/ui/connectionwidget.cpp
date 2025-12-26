//
// Created by hstasonis on 12/26/25.
//

#include "Wildcat/ui/connectionwidget.h"

DeviceConnectionWidget::DeviceConnectionWidget(QWidget* parent)
{
  m_layout = new QHBoxLayout(this);

  m_connectionLabel = new QLabel();
  deviceDisconnected();

  m_layout->addWidget(m_connectionLabel);
}

void DeviceConnectionWidget::deviceConnected() const
{
  m_connectionLabel->setText("Connected");
  m_connectionLabel->setStyleSheet("color: rgb(102, 255, 0);");
}

void DeviceConnectionWidget::deviceDisconnected() const
{
  m_connectionLabel->setText("Disconnected");
  m_connectionLabel->setStyleSheet("color: #F08080;");
}
