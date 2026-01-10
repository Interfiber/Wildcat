//
// Created by hstasonis on 12/26/25.
//

#pragma once
#include <QMainWindow>

class ImportWindow;
class DeviceConnectionWidget;
class WildcatDevice;
class ChannelsWidget;

/**
 * Main user interface window
 */
class WildcatMainWindow : public QMainWindow
{
public:
  WildcatMainWindow();
  ~WildcatMainWindow() override;

public slots:
  void connectToDevice();
  void alertWarning(const std::string& content);

private:
  void initMenuBar();

  // File
  QAction* ma_connectToDevice = nullptr;
  QAction* ma_loadFromFile = nullptr;
  QAction* ma_saveToFile = nullptr;
  QAction* ma_saveAsToFile = nullptr;

  // Edit
  QAction* ma_newChannel = nullptr;
  QAction* ma_deleteChannel = nullptr;
  QAction* ma_paste = nullptr;

  // Device
  QAction* ma_setSquelch = nullptr;
  QAction* ma_setVolume = nullptr;
  QAction* ma_resetMemory = nullptr;
  QAction* ma_writeChannels = nullptr;

  // Help

  QAction* ma_aboutQt = nullptr;
  QAction* ma_aboutWildcat = nullptr;

  // UI widgets
  ChannelsWidget* m_channelsWidget;
  DeviceConnectionWidget* m_connectionWidget;
  ImportWindow* m_importWindow;

  friend class ChannelsWidget;
  friend class ImportWindow;
};
