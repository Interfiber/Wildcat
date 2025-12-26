//
// Created by hstasonis on 12/26/25.
//

#include <Wildcat/ui/channelswidget.h>
#include "Wildcat/io/device.h"
#include "Wildcat/io/channel.h"
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

#include "Wildcat/io/ctcss.h"
#include "Wildcat/ui/mainwindow.h"

ChannelsWidget::ChannelsWidget(QWidget* parent) : QWidget(parent)
{
    // Init UI
    m_layout = new QHBoxLayout(this);

    m_tabWidget = new QTabWidget(this);

    m_banks.reserve(WildcatDevice::MAX_BANKS);

    for (int i = 0; i < WildcatDevice::MAX_BANKS; i++)
    {
        auto table = new QTableWidget(0, 7); // Start off with zero channels programmed
        table->setHorizontalHeaderLabels({ "Name", "Frequency", "Modulation", "CTCSS/DCS", "Lockout", "Delay", "Priority" });
        table->setAlternatingRowColors(true);
        table->setSelectionMode(QAbstractItemView::SingleSelection);

        m_banks.push_back(table);

        m_tabWidget->addTab(table, ("Bank #" + std::to_string(i + 1)).data());
    }

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
    auto mainWindow = static_cast<WildcatMainWindow*>(parent);

    connect(m_writeToDevice, &QPushButton::clicked, mainWindow->ma_writeChannels, &QAction::trigger);

    m_quickActionsLayout->addWidget(m_writeToDevice);

    // Load from device quick action

    m_loadFromDevice = new QPushButton(nullptr);
    m_loadFromDevice->setText("Load from device");
    m_loadFromDevice->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::DocumentOpen));
    m_loadFromDevice->setSizePolicy(spRight);

    m_quickActionsLayout->addWidget(m_loadFromDevice);

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

void ChannelsWidget::addChannel()
{
    if (WildcatMainWindow::get()->m_device == nullptr)
        return;

    auto table = static_cast<QTableWidget*>(m_tabWidget->currentWidget());

    if (table->rowCount() + 1 > WildcatDevice::MAX_CHANNELS_PER_BANK)
    {
       QMessageBox::warning(this, "Wildcat", ("This bank is full (" + std::to_string(WildcatDevice::MAX_CHANNELS_PER_BANK) + " channels per bank), please either clear out frequencies or switch to another bank.").data());

        return;
    }

    table->setRowCount(table->rowCount() + 1);
    table->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    table->scrollToBottom();

    const int rowCount = table->rowCount() - 1;

    UIChannel channel{};
    channel.channel = WildcatMainWindow::get()->m_device->newChannel();

    channel.channel->bank = m_tabWidget->currentIndex();

    constexpr float floatMax = std::numeric_limits<float>::max();
    const QDoubleValidator* dv = new QDoubleValidator(-floatMax, floatMax, 4);

    // Name

    channel.name = new QLineEdit(nullptr);
    channel.name->setPlaceholderText("Channel name");
    channel.name->setMaxLength(16);

    connect(channel.name, &QLineEdit::textChanged, this, [channel](const QString& text)
    {
        channel.channel->name = text.toStdString();
    });

    // Frequency

    channel.freq = new QLineEdit(nullptr);
    channel.freq->setPlaceholderText("Frequency (MHz)");
    channel.freq->setValidator(dv);

    connect(channel.freq, &QLineEdit::textChanged, this, [channel](const QString& text)
    {
        channel.channel->frequency = std::stof(text.toStdString());
    });

    // Modulation

    channel.modulation = new QComboBox(nullptr);
    channel.modulation->addItems({
    "Automatic","AM", "FM", "NFM"
    });

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

    // Lockout

    channel.lockout = new QComboBox(nullptr);
    channel.lockout->addItems({
    "Off", "Lockout"
    });

    connect(channel.lockout, &QComboBox::currentIndexChanged, this, [channel](const int index)
    {
        channel.channel->lockoutMode = index == 0 ? WildcatChannel::LockoutMode::Off : WildcatChannel::LockoutMode::Lockout;
    });

    // Delay

    channel.delay = new QComboBox(nullptr);
    channel.delay->addItems(WildcatChannel::DELAY_VALUES);
    channel.delay->setCurrentIndex(WildcatChannel::DELAY_VALUES.indexOf("2"));

    connect(channel.delay, &QComboBox::currentIndexChanged, this, [channel](const int index)
    {
        channel.channel->delay = std::stoi(WildcatChannel::DELAY_VALUES.at(index).toStdString());
    });

    // Priority

    channel.priority = new QComboBox(nullptr);
    channel.priority->addItems({ "Off", "PCH" });

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