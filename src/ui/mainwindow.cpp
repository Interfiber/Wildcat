//
// Created by hstasonis on 12/26/25.
//

#include <QAction>
#include <QApplication>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <Wildcat/fs/csv.h>
#include <Wildcat/fs/pastedsheet.h>
#include <Wildcat/ui/mainwindow.h>

#include <QClipboard>
#include "Wildcat/global.h"
#include "Wildcat/io/device.h"
#include "Wildcat/ui/channelswidget.h"
#include "Wildcat/ui/connectionwidget.h"
#include "Wildcat/ui/devicepicker.h"

#include "Wildcat/fs/archive.h"
#include "Wildcat/ui/importwindow.h"

WildcatMainWindow::WildcatMainWindow()
{
  setWindowTitle(("Wildcat " + std::string(GIT_BRANCH_BUILD) + "@" + std::string(GIT_HASH_BUILD)).data());

  statusBar()->showMessage("Welcome to Wildcat v2!");

  resize(800, 600);

  // Register archive formats for importing/exporting

  WildcatArchiveImporter::get()->addArchivePair<WildcatPastedSheetArchive>();
  WildcatArchiveImporter::get()->addArchivePair<WildcatCSVArchive>();

  // Import window (early init)

  m_importWindow = new ImportWindow();

  // Init the menu bar

  initMenuBar();

  // Main UI

  m_channelsWidget = new ChannelsWidget(this);
  m_connectionWidget = new DeviceConnectionWidget();

  menuBar()->setCornerWidget(m_connectionWidget, Qt::TopRightCorner);

  // Connect menu bar actions to widgets

  connect(ma_newChannel, &QAction::triggered, this, [this] { m_channelsWidget->addChannel(nullptr); });

  // Connect showWarning threaded signal

  connect(
    WildcatGlobalState::get().get(), &WildcatGlobalState::showWarning, this,
    [](const std::string& msg) { QMessageBox::warning(nullptr, "Wildcat", msg.data()); }, Qt::BlockingQueuedConnection);

  setCentralWidget(m_channelsWidget);
  setWindowIcon(QIcon(":/resources/wcat2.png"));
}

WildcatMainWindow::~WildcatMainWindow() { delete m_channelsWidget; }

void
WildcatMainWindow::connectToDevice()
{
  if (DEVICE == nullptr)
  {
    if (DevicePickerDialog devicePicker(this); devicePicker.exec() == QDialog::Accepted)
    {
      DEVICE = std::make_shared<WildcatDevice>(devicePicker.getSelectedDevice());
    }
    else
    {
      return;
    }
  }
  else
  {
    DEVICE->reconnect();
  }

  if (!DEVICE->isConnected())
  {
    m_connectionWidget->deviceDisconnected();

    return;
  }

  connect(DEVICE.get(), &WildcatDevice::deviceStatusChanged, m_connectionWidget,
          &DeviceConnectionWidget::deviceStatusChanged);
  connect(DEVICE.get(), &WildcatDevice::deviceErased, m_channelsWidget, &ChannelsWidget::clearChannels);

  m_connectionWidget->deviceConnected();
  m_channelsWidget->loadCurrentBank();

  const WildcatDevice::Info info = DEVICE->getInfo();

  connect(ma_writeChannels, &QAction::triggered, this,
          [this]
          {
            if (DEVICE == nullptr)
              return;

            DEVICE->updateChannels();

            statusBar()->showMessage("Wrote channels to device!");
          });

  connect(
    ma_resetMemory, &QAction::triggered, this,
    [this]
    {
      if (const QMessageBox::StandardButton button = QMessageBox::question(
            this, "Wildcat",
            "Do you really wish to erase the memory from this device?\nNOTE: All channels and settings will be lost!",
            QMessageBox::Yes | QMessageBox::Abort, QMessageBox::Abort);
          button == QMessageBox::StandardButton::Yes)
      {
        if (!DEVICE->isConnected())
          return;

        DEVICE->clearMemory();
      }
    });

  QMessageBox::information(nullptr, "Wildcat",
                           ("Connected to device " + info.model + " running firmware " + info.firmware).data());

  statusBar()->showMessage(("Connected to " + info.model).data());
}

void
WildcatMainWindow::alertWarning(const std::string& content)
{
  QMessageBox::warning(this, "Wildcat", content.data());
}

void
WildcatMainWindow::initMenuBar()
{
  // Create actions

  ma_connectToDevice = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::CallStart), "Connect to serial device");
  ma_loadFromFile = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen), "Load channels from file");

  ma_paste = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditPaste), "Paste (auto-convert)");
  ma_newChannel = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentNew), "New channel");
  ma_deleteChannel = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditDelete), "Delete channel");

  ma_setSquelch = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::AudioVolumeMuted), "Set squelch");
  ma_setVolume = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::AudioVolumeHigh), "Set volume");
  ma_resetMemory = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::EditClear), "Reset memory");
  ma_writeChannels = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSend), "Write channels");

  ma_aboutQt = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::HelpAbout), "About Qt");
  ma_aboutWildcat = new QAction(QIcon::fromTheme(QIcon::ThemeIcon::HelpAbout), "About Wildcat");

  // Set shortcuts

  ma_paste->setShortcut(QKeySequence::Paste);
  ma_newChannel->setShortcut(tr("Ctrl+Shift+N"));
  ma_deleteChannel->setShortcut(QKeySequence::Delete);

  ma_loadFromFile->setShortcut(QKeySequence::Open);

  QMenu* file = menuBar()->addMenu("File");

  file->addAction(ma_connectToDevice);
  file->addAction(ma_loadFromFile);

  QMenu* edit = menuBar()->addMenu("Edit");

  edit->addAction(ma_paste);
  edit->addSeparator();
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

  connect(ma_aboutWildcat, &QAction::triggered, this,
          [this]
          {
            QMessageBox::information(this, "About Wildcat",
                                     ("Wildcat v2\nGit branch: " + std::string(GIT_BRANCH_BUILD)
                                      + "\nGit hash: " + std::string(GIT_HASH_BUILD)
                                      + "\nGitHub: https://github.com/Interfiber/Wildcat.git")
                                       .data());
          });

  connect(ma_paste, &QAction::triggered, this, [this]() { m_importWindow->openFromPaste(m_channelsWidget); });

  // Device specific connects in connectToDevice()
}
