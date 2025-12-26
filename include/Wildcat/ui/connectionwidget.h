//
// Created by hstasonis on 12/26/25.
//

#include <qboxlayout.h>
#include <QLabel>
#include <qwidget.h>

/**
 * Displays the device connection state
 */
class DeviceConnectionWidget : public QWidget
{
public:
  explicit DeviceConnectionWidget(QWidget* parent = 0);

public slots:
  void deviceConnected() const;
  void deviceDisconnected() const;

private:
  QHBoxLayout* m_layout;
  QLabel* m_connectionLabel;
};