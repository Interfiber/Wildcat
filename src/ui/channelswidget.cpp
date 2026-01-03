//
// Created by hstasonis on 12/26/25.
//

#include <Wildcat/ui/channelswidget.h>
#include "Wildcat/global.h"
#include "Wildcat/io/device.h"
#include "Wildcat/io/channel.h"
#include <QCheckBox>
#include <QLineEdit>
#include <qmenu.h>
#include <QMessageBox>
#include <QPushButton>

#include "Wildcat/io/ctcss.h"
#include "Wildcat/io/iothread.h"
#include "Wildcat/ui/mainwindow.h"
#include "Wildcat/ui/textinput.h"

BankLoaderThread::BankLoaderThread(const std::shared_ptr<WildcatDevice>& device, const int bank) : QThread(nullptr)
{
    m_device = device;
    m_bank = bank;
}

void BankLoaderThread::run()
{
    std::vector<WildcatDevice::DeviceResult<WildcatChannel>> channelResults;

    for (int i = 0; i < WildcatDevice::MAX_CHANNELS_PER_BANK; i++)
    {
        channelResults.push_back(m_device->getChannelAsync(i + 1, m_bank));
    }

    for (auto &channel : channelResults)
    {
        WildcatChannel c = channel.wait().unwrap();

        // Skip empty channels
        if (c.name.empty()) break;

        auto c2 = std::make_shared<WildcatChannel>(c);
        c2->index /= m_bank;
        c2->bank= m_bank;

        m_device->addChannel(c2);

        requestNewChannel(c2);
    }
}

ChannelsWidget::ChannelsWidget(QWidget* parent) : QWidget(parent)
{
    // Init UI
    m_layout = new QHBoxLayout(this);

    // Context menu
    m_setBankNickname = new QAction("Set nickname");

    connect(m_setBankNickname, &QAction::triggered, this, [this]
    {
        if (TextInputDialog dialog("Enter nickname for bank", this); dialog.exec() == QDialog::Accepted)
        {
            const int cTab = m_tabWidget->currentIndex();

            m_settings.beginGroup("Banks");

            m_settings.setValue(("bank" + std::to_string(cTab)).data(), QString(dialog.getResult().data()));

            m_settings.endGroup();

            m_tabWidget->setTabText(cTab, dialog.getResult().data());
        }
    });

    m_contextMenu = new QMenu(nullptr);

    m_contextMenu->addAction(m_setBankNickname);
    m_contextMenu->addAction("Delete bank");

    // Tab bar / table

    m_tabWidget = new QTabWidget(this);

    m_banks.reserve(WildcatDevice::MAX_BANKS);

    for (int i = 0; i < WildcatDevice::MAX_BANKS; i++)
    {
        auto table = new QTableWidget(0, 7); // Start off with zero channels programmed
        table->setHorizontalHeaderLabels({ "Name", "Frequency", "Modulation", "CTCSS/DCS", "Lockout", "Delay", "Priority" });
        table->setAlternatingRowColors(true);
        table->setSelectionMode(QAbstractItemView::SingleSelection);

        m_banks.push_back(table);

        // Check for custom nicknames, if one exists then we can use it!

        QString bankName = ("Bank #" + std::to_string(i + 1)).data();

        m_settings.beginGroup("Banks");

        if (m_settings.contains("bank" + std::to_string(i)))
        {
            bankName = m_settings.value("bank" + std::to_string(i)).toString();
        }

        m_settings.endGroup();

        // Add new tab for the bank
        m_tabWidget->addTab(table, bankName);
    }

    connect(m_tabWidget, &QTabWidget::currentChanged, this, [this] (int index)
    {
        if (!HOTLOAD) return;

        // Only load empty banks
        if (static_cast<QTableWidget*>(m_tabWidget->currentWidget())->rowCount() == 0)
            loadCurrentBank();
    });

    connect(m_tabWidget, &QTabWidget::tabBarDoubleClicked, this, [this] (int index)
    {
        m_contextMenu->popup(QCursor::pos());
    });

    QSizePolicy spLeft(QSizePolicy::Preferred, QSizePolicy::Preferred);
    spLeft.setHorizontalStretch(7);

    m_tabWidget->setSizePolicy(spLeft);

    m_layout->addWidget(m_tabWidget);

    // Quick actions

    m_quickActionsLayout = new QVBoxLayout();
    m_quickActionsLayout->setAlignment(Qt::AlignTop);

    QSizePolicy spRight(QSizePolicy::Preferred, QSizePolicy::Preferred);
    spRight.setHorizontalStretch(1);

    // Title bar of quick actionss

    m_quickActionsLayout->addSpacing(20);

    m_quickActionsLabel = new QLabel(nullptr);
    m_quickActionsLabel->setText("Quick actions");

    m_quickActionsLayout->addWidget(m_quickActionsLabel);

    // Write to device quick action

    m_writeToDevice = new QPushButton(nullptr);
    m_writeToDevice->setText("Write to device");
    m_writeToDevice->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentSend));
    m_writeToDevice->setSizePolicy(spRight);

    // Parent should always be the main window
    const auto mainWindow = static_cast<WildcatMainWindow*>(parent);

    connect(m_writeToDevice, &QPushButton::clicked, mainWindow->ma_writeChannels, &QAction::trigger);

    // Load from device quick action

    m_enableHotload = new QCheckBox(nullptr);
    m_enableHotload->setText("Enable hot channel loading?");

    connect(m_enableHotload, &QCheckBox::clicked, this, [this, parent]
    {
        HOTLOAD = m_enableHotload->isChecked();

        if (m_enableHotload->isChecked())
        {
            QMessageBox::information(this, "Wildcat", "Enabled hot loading of channels!\n\nWhen an empty bank is viewed for the first time Wildcat will attempt to load it from the scanner");
        }
    });

    // Add widgets

    m_quickActionsLayout->addWidget(m_enableHotload);
    m_quickActionsLayout->addWidget(m_writeToDevice);

    m_quickActions = new QWidget();
    m_quickActions->setSizePolicy(spRight);
    m_quickActions->setLayout(m_quickActionsLayout);

    m_layout->addWidget(m_quickActions);
}

ChannelsWidget::~ChannelsWidget()
{
    m_channels.clear();

    delete m_layout;
}

void ChannelsWidget::UIChannel::destroy() const
{
    delete name;
    delete freq;
    delete modulation;
    delete ctcss;
    delete delay;
    delete lockout;
    delete priority;
}

void ChannelsWidget::addChannel(const std::shared_ptr<WildcatChannel> &precacheChannel)
{
    if (DEVICE == nullptr)
        return;

    auto table = static_cast<QTableWidget*>(m_tabWidget->currentWidget());

    if (table->rowCount() + 1 > WildcatDevice::MAX_CHANNELS_PER_BANK)
    {
        return;
    }

    table->setRowCount(table->rowCount() + 1);
    table->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    const int rowCount = table->rowCount() - 1;

    // Create the new channel

    UIChannel channel{};

    // Determine which channel to use
    channel.channel = precacheChannel == nullptr ? DEVICE->newChannel() : precacheChannel;

    channel.channel->index = rowCount + 1;
    channel.channel->bank = m_tabWidget->currentIndex() + 1;

    constexpr float floatMax = std::numeric_limits<float>::max();
    const QDoubleValidator* dv = new QDoubleValidator(-floatMax, floatMax, 4);

    // Name

    channel.name = new QLineEdit(nullptr);
    channel.name->setPlaceholderText("Channel name");
    channel.name->setMaxLength(16);
    channel.name->setText(channel.channel->name.data());

    connect(channel.name, &QLineEdit::textChanged, this, [channel](const QString& text)
    {
        channel.channel->name = text.toStdString();
    });

    // Frequency

    channel.freq = new QLineEdit(nullptr);
    channel.freq->setPlaceholderText("Frequency (MHz)");
    channel.freq->setValidator(dv);
    channel.freq->setText(QString::number(channel.channel->frequency));

    connect(channel.freq, &QLineEdit::textChanged, this, [channel](const QString& text)
    {
        try
        {
            channel.channel->frequency = std::stof(text.toStdString());
        } catch (std::exception &e)
        {
            printf("Failed to convert input string to frequency (float): %s\n", e.what());
        }
    });

    // Modulation

    channel.modulation = new QComboBox(nullptr);
    channel.modulation->addItems({
    "Automatic","AM", "FM", "NFM"
    });
    channel.modulation->setCurrentText(WildcatChannel::modulationModeToString(channel.channel->modulation).data());

    connect(channel.modulation, &QComboBox::currentIndexChanged, this, [channel](const int index)
    {
        switch (index)
        {
        case 0:
            channel.channel->modulation = WildcatChannel::ModulationMode::Automatic;
            break;
        case 1:
            channel.channel->modulation = WildcatChannel::ModulationMode::AM;
            break;
        case 2:
            channel.channel->modulation = WildcatChannel::ModulationMode::FM;
            break;
        case 3:
            channel.channel->modulation = WildcatChannel::ModulationMode::NFM;
            break;
        default:
            break;
        }
    });


    // CTCSS / DCS

    channel.ctcss = new QComboBox(nullptr);
    channel.ctcss->addItems(Wildcat_GetCTCSSCodes());
    channel.ctcss->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    // FIXME: Impl CTCSS/DCS

    // Lockout

    channel.lockout = new QComboBox(nullptr);
    channel.lockout->addItems({
    "Off", "Lockout"
    });

    channel.lockout->setCurrentText(channel.channel->lockoutMode == WildcatChannel::LockoutMode::Off ? "Off" : "Lockout");

    connect(channel.lockout, &QComboBox::currentIndexChanged, this, [channel](const int index)
    {
        channel.channel->lockoutMode = index == 0 ? WildcatChannel::LockoutMode::Off : WildcatChannel::LockoutMode::Lockout;
    });

    // Delay

    channel.delay = new QComboBox(nullptr);
    channel.delay->addItems(WildcatChannel::DELAY_VALUES);
    channel.delay->setCurrentIndex(WildcatChannel::DELAY_VALUES.indexOf(std::to_string(channel.channel->delay)));

    connect(channel.delay, &QComboBox::currentIndexChanged, this, [channel](const int index)
    {
        channel.channel->delay = std::stoi(WildcatChannel::DELAY_VALUES.at(index).toStdString());
    });

    // Priority

    channel.priority = new QComboBox(nullptr);
    channel.priority->addItems({ "Off", "PCH" });
    channel.priority->setCurrentText(channel.channel->priority == WildcatChannel::PriorityMode::Off ? "Off" : "PCH");

    connect(channel.priority, &QComboBox::currentIndexChanged, this, [channel](const int index)
    {
        channel.channel->priority = index == 0 ? WildcatChannel::PriorityMode::Off : WildcatChannel::PriorityMode::PCH;
    });

    m_channels.push_back(channel);

    // Insert channel into the UI
    table->setCellWidget(rowCount, 0, channel.name);
    table->setCellWidget(rowCount, 1, channel.freq);
    table->setCellWidget(rowCount, 2, channel.modulation);
    table->setCellWidget(rowCount, 3, channel.ctcss);
    table->setCellWidget(rowCount, 4, channel.lockout);
    table->setCellWidget(rowCount, 5, channel.delay);
    table->setCellWidget(rowCount, 6, channel.priority);

    table->resizeColumnsToContents();
    table->resizeRowsToContents();
}

void ChannelsWidget::loadCurrentBank()
{
    if (!DEVICE->isConnected())
    {
        QMessageBox::warning(this, "Wildcat", "Device has disconnected!");
        return;
    }

    const int bank = m_tabWidget->currentIndex() + 1;

    if (m_loadedBanks.find(bank) != m_loadedBanks.end() && m_loadedBanks[bank])
        return;

    m_loadedBanks.insert({ bank, true });

    auto display = new WildcatIOStatusDisplay(this);
    display->show();

    // Shut up CLion this object is deleted by Qt when the thread exits
    const auto loader = new BankLoaderThread(DEVICE, bank);

    connect(loader, &BankLoaderThread::requestNewChannel, this, &ChannelsWidget::addChannel);
    connect(loader, &QThread::finished, loader, &QObject::deleteLater);
    connect(loader, &QThread::finished, this, [display]
    {
        display->close();

        delete display;
    });

    loader->start();
}

void ChannelsWidget::clearChannels()
{
    for (int i = 0; i < m_tabWidget->count(); i++)
    {
        const auto table = dynamic_cast<QTableWidget*>(m_tabWidget->widget(i));
        table->clearContents();
        table->setRowCount(0);

        table->resizeColumnsToContents();
        table->resizeRowsToContents();
    }

    for (auto &c : m_channels)
    {
        c.destroy();
    }

    m_channels.clear();
}
