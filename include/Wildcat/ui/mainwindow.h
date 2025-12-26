//
// Created by hstasonis on 12/26/25.
//

#pragma once
#include <QMainWindow>

class WildcatDevice;
class ChannelsWidget;

/**
 * Main user interface window
 */
class WildcatMainWindow : public QMainWindow
{
public:
    WildcatMainWindow();

public slots:
    void connectToDevice();

private:
    void initMenuBar();

    // File
    QAction *ma_connectToDevice = nullptr;
    QAction *ma_loadFromFile = nullptr;
    QAction *ma_saveToFile = nullptr;
    QAction *ma_saveAsToFile = nullptr;

    // Edit
    QAction *ma_undo = nullptr;
    QAction *ma_redo = nullptr;

    QAction* ma_newChannel = nullptr;
    QAction* ma_deleteChannel = nullptr;

    // Device
    QAction* ma_setSquelch = nullptr;
    QAction* ma_setVolume = nullptr;
    QAction* ma_resetMemory = nullptr;
    QAction* ma_writeChannels = nullptr;

    // Help

    QAction* ma_aboutQt;
    QAction* ma_aboutWildcat;

    ChannelsWidget *m_channelsWidget;

    std::shared_ptr<WildcatDevice> m_device;
};