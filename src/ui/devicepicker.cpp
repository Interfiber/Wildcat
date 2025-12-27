//
// Created by hstasonis on 12/26/25.
//

#include "Wildcat/ui/devicepicker.h"

#include <qicon.h>
#include <QPushButton>

#include "Wildcat/io/device.h"

DevicePickerDialog::DevicePickerDialog(QWidget *parent) : QDialog(parent)
{
  setWindowTitle("Wildcat device picker");

  m_layout = new QVBoxLayout();


  // Device selector

  m_title = new QLabel("Select a serial device to connect to:");

  m_deviceSelector = new QComboBox(nullptr);

  for (auto &device : WildcatDevice::getConnectableDevices())
  {
    m_deviceSelector->addItem(device.data());
  }

  connect(m_deviceSelector, &QComboBox::currentIndexChanged, this, [this]
  {
    m_selectedDevice = m_deviceSelector->currentText().toStdString();
  });

  m_selectedDevice = m_deviceSelector->currentText().toStdString();

  // Bottom buttons

  m_accept = new QPushButton("Connect to device");
  m_accept->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen));
  m_accept->setDefault(true);

  connect(m_accept, &QPushButton::clicked, this, &DevicePickerDialog::accept);

  m_deny = new QPushButton("Cancel");

  connect(m_deny, &QPushButton::clicked, this, &DevicePickerDialog::reject);

  m_buttonLayout = new QHBoxLayout();
  m_buttonLayout->addWidget(m_accept);
  m_buttonLayout->addWidget(m_deny);

  // Assemble UI

  m_layout->addWidget(m_title);
  m_layout->addWidget(m_deviceSelector);
  m_layout->addLayout(m_buttonLayout);

  setLayout(m_layout);
}

DevicePickerDialog::~DevicePickerDialog()
{
  delete m_title;
  delete m_deviceSelector;
  delete m_accept;
  delete m_deny;

  delete m_buttonLayout;
  delete m_layout;
}
