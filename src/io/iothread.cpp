//
// Created by hstasonis on 12/28/25.
//

#include <Wildcat/io/iothread.h>

WildcatIOThread::WildcatIOThread(WildcatDevice* device)
{
    m_device = device;

    m_thread = std::thread([this]
    {
        run();
    });

    m_thread.detach();
}

WildcatDevice::DeviceResult<std::string>& WildcatIOThread::issueAsyncWrite(const std::string& buffer)
{
    std::lock_guard<std::mutex> _g(m_queueLock);

    WildcatDevice::DeviceResult<std::string> result{};
    result.async.isAsync = true;
    result.async.hasCompleted = false;

    // Add the write operation

    WriteOperation op{};
    op.writeBuffer = buffer;
    op.result = result;

    m_writes.push_back(op);

    return m_writes[m_writes.size() - 1].result;
}

[[noreturn]] void WildcatIOThread::run()
{
    printf("Wildcat IO thread started!\n");

    while (true)
    {
        std::lock_guard<std::mutex> _g(m_queueLock);

        // Don't hog CPU time
        std::this_thread::sleep_for(std::chrono::milliseconds(30));

        for (auto &op : m_writes)
        {
            printf("Performing write operation on IO thread...\n");

            WildcatDevice::DeviceResult<std::string> result = m_device->issueAsync(op.writeBuffer);
            result.async.isAsync = true;
            result.async.hasCompleted = true;

            op.result =  result;
        }

        m_writes.clear();
    }
}

WildcatIOStatusDisplay::WildcatIOStatusDisplay()
{
    m_title = new QLabel("Communicating with serial device...");

    m_progress = new QProgressBar();
    m_progress->setMinimum(0);
    m_progress->setMaximum(0);

    m_layout = new QVBoxLayout();

    m_layout->addWidget(m_title);
    m_layout->addWidget(m_progress);
}