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

    static WildcatMainWindow* get()
    {
        static auto instance = new WildcatMainWindow();

        return instance;
    }

    /// @brief  When enabled, empty banks will be automatically loaded from the scanner
    bool hotload = false;

public slots:
    void connectToDevice();
    void alertWarning(const std::string &content);

private:
    void initMenuBar();

    // File
    QAction *ma_connectToDevice = nullptr;
    QAction *ma_loadFromFile = nullptr;
    QAction *ma_saveToFile = nullptr;
    QAction *ma_saveAsToFile = nullptr;

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

    ChannelsWidget *m_channelsWidget;
    DeviceConnectionWidget* m_connectionWidget;
    ImportWindow* m_importWindow;

    std::shared_ptr<WildcatDevice> m_device;

    friend class ChannelsWidget;
    friend class ImportWindow;
    friend class WildcatPastedSheetArchive;
};
