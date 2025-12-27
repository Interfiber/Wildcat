//
// Created by hstasonis on 12/26/25.
//

#pragma once
#include <QComboBox>
#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>


/**
 * Handles picking a device to connect to
 */
class DevicePickerDialog : public QDialog
{
public:
  explicit DevicePickerDialog(QWidget *parent = nullptr);
  ~DevicePickerDialog() override;

  /// @brief  Return selected device
  [[nodiscard]] std::string getSelectedDevice() const
  {
    return m_selectedDevice;
  }

private:
  std::string m_selectedDevice;

  QVBoxLayout *m_layout;

  QHBoxLayout *m_buttonLayout;
  QPushButton *m_accept;
  QPushButton *m_deny;

  QLabel *m_title;
  QComboBox *m_deviceSelector;
};
