//
// Created by hstasonis on 12/28/25.
//

#pragma once
#include <qboxlayout.h>
#include <QDialog>
#include <QProgressBar>
#include <thread>
#include <QLabel>
#include "Wildcat/io/device.h"
#include "Wildcat/io/message.h"

/**
 * Handles performing all IO operations for a device on a separate thread so the UI can remain alive
 */
class WildcatIOThread
{
public:
    explicit WildcatIOThread(WildcatDevice* device);

     /**
     * Write a buffer to the device async
     * @param buffer Buffer to write
     * @return Future result of the write operation
     */
    WildcatDevice::DeviceResult<std::string> issueAsyncWrite(const std::string &buffer);

signals:
    /// @brief  Called at the start of an IO operation
    void ioOperationBegin();

    /// @brief  Called at the end of an IO operation
    void ioOperationEnd();

private:
    [[noreturn]] void run();

    std::mutex m_queueLock;

    struct WriteOperation
    {
        WildcatDevice::DeviceResult<std::string> result;

        std::string writeBuffer;
    };

    /// @brief  List of write operations to perform
    std::vector<WriteOperation> m_writes;

    WildcatDevice* m_device;
    std::thread m_thread;

    friend class WildcatIOStatusDisplay;
};

/**
 * Display the status of IO operations
 */
class WildcatIOStatusDisplay : public QDialog
{
public:
    explicit WildcatIOStatusDisplay(QWidget *parent = nullptr);
    ~WildcatIOStatusDisplay() override;

private:
    QVBoxLayout *m_layout;
    QProgressBar* m_progress;
    QLabel* m_title;
};