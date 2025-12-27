//
// Created by hstasonis on 12/26/25.
//

#include <Wildcat/ui/mainwindow.h>
#include <QStatusBar>
#include <QMenuBar>
#include <QAction>
#include <QApplication>
#include <QMessageBox>

#include "Wildcat/io/device.h"
#include "Wildcat/ui/channelswidget.h"
#include "Wildcat/ui/connectionwidget.h"
#include "Wildcat/ui/devicepicker.h"

WildcatMainWindow::WildcatMainWindow()
{
    setWindowTitle(("Wildcat " + std::string(GIT_BRANCH_BUILD) + "@" + std::string(GIT_HASH_BUILD)).data());

    statusBar()->showMessage("Welcome to Wildcat v2!");

    resize(800, 600);

    // Init the menu bar

    initMenuBar();

    // Main UI

    m_channelsWidget = new ChannelsWidget(this);
    m_connectionWidget = new DeviceConnectionWidget();

    menuBar()->setCornerWidget(m_connectionWidget, Qt::TopRightCorner);

    // Connect menu bar actions to widgets

    connect(ma_newChannel, &QAction::triggered, m_channelsWidget, &ChannelsWidget::addChannel);

    setCentralWidget(m_channelsWidget);
}

WildcatMainWindow::~WildcatMainWindow()
{
    delete m_channelsWidget;
}

void WildcatMainWindow::connectToDevice()
{
    if (m_device == nullptr)
    {
        if (DevicePickerDialog devicePicker(this); devicePicker.exec() == QDialog::Accepted)
        {
            m_device = std::make_shared<WildcatDevice>(devicePicker.getSelectedDevice());
        }
        else
        {
            return;
        }
    }
    else
    {
        m_device->reconnect();
    }

    if (!m_device->isConnected())
    {
        m_connectionWidget->deviceDisconnected();

        return;
    }

    connect(m_device.get(), &WildcatDevice::deviceStatusChanged, m_connectionWidget, &DeviceConnectionWidget::deviceStatusChanged);

    m_connectionWidget->deviceConnected();

    const WildcatDevice::Info info = m_device->getInfo();

    connect(ma_writeChannels, &QAction::triggered, this, [this]
    {
        if (m_device == nullptr) return;

        m_device->updateChannels();

        statusBar()->showMessage("Wrote channels to device!");
    });

    QMessageBox::information(nullptr, "Wildcat", ("Connected to device " + info.model + " running firmware " + info.firmware).data());

    statusBar()->showMessage(("Connected to " + info.model).data());
}

void WildcatMainWindow::alertWarning(const std::string& content)
{
    QMessageBox::warning(this, "Wildcat", content.data());
}

void WildcatMainWindow::initMenuBar()
{
    // Create actions

    ma_connectToDevice = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::CallStart), "Connect to serial device");
    ma_loadFromFile = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), "Load channels from file");

    ma_undo = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditUndo), "Undo action");
    ma_redo = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditRedo), "Redo action");
    ma_newChannel = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew), "New channel");
    ma_deleteChannel = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditDelete), "Delete channel");

    ma_setSquelch = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::AudioVolumeMuted), "Set squelch");
    ma_setVolume = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::AudioVolumeHigh), "Set volume");
    ma_resetMemory = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditClear), "Reset memory");
    ma_writeChannels = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSend), "Write channels");

    ma_aboutQt = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::HelpAbout), "About Qt");
    ma_aboutWildcat = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::HelpAbout), "About Wildcat");

    // Set shortcuts

    ma_newChannel->setShortcut(tr("Ctrl+Shift+N"));
    ma_deleteChannel->setShortcut(QKeySequence::Delete);

    ma_undo->setShortcut(QKeySequence::Undo);
    ma_redo->setShortcut(QKeySequence::Redo);

    ma_loadFromFile->setShortcut(QKeySequence::Open);

    QMenu* file = menuBar()->addMenu("File");

    file->addAction(ma_connectToDevice);
    file->addAction(ma_loadFromFile);

    QMenu* edit = menuBar()->addMenu("Edit");

    edit->addAction(ma_undo);
    edit->addAction(ma_redo);
    edit->addAction(ma_newChannel);
    edit->addAction(ma_deleteChannel);

    QMenu* device = menuBar()->addMenu("Device");

    device->addAction(ma_setSquelch);
    device->addAction(ma_setVolume);
    device->addAction(ma_resetMemory);
    device->addAction(ma_writeChannels);

    QMenu* help = menuBar()->addMenu("Help");

    help->addAction(ma_aboutWildcat);
    help->addAction(ma_aboutQt);

    // Connections

    connect(ma_aboutQt, &QAction::triggered, this, QApplication::aboutQt);
    connect(ma_connectToDevice, &QAction::triggered, this, &WildcatMainWindow::connectToDevice);

    // Device specific connects in connectToDevice()
}
