//
// Created by hstasonis on 12/26/25.
//

#include <Wildcat/ui/channelswidget.h>
#include "Wildcat/io/device.h"
#include "Wildcat/io/channel.h"
#include <QLineEdit>

#include "Wildcat/io/ctcss.h"
#include "Wildcat/ui/mainwindow.h"

ChannelsWidget::ChannelsWidget(QWidget* parent) : QWidget(parent)
{
    // Init UI
    m_layout = new QVBoxLayout(this);

    m_table = new QTableWidget(0, 7); // Start off with zero channels programmed
    m_table->setHorizontalHeaderLabels({ "Name", "Frequency", "Modulation", "CTCSS/DCS", "Lockout", "Delay", "Priority" });
    m_table->setAlternatingRowColors(true);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);

    m_layout->addWidget(m_table);

    //m_noDevice = new QLabel("Use 'File -> Connect to serial device' to get started!");

    //m_layout->addWidget(m_noDevice, 0, Qt::AlignCenter);
}

ChannelsWidget::~ChannelsWidget()
{
    m_channels.clear();

    delete m_table;
    delete m_layout;
}

void ChannelsWidget::addChannel()
{
    if (WildcatMainWindow::get()->m_device == nullptr)
        return;

    m_table->setRowCount(m_table->rowCount() + 1);
    m_table->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

    const int rowCount = m_table->rowCount() - 1;

    UIChannel channel{};
    channel.channel = WildcatMainWindow::get()->m_device->newChannel();

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
    m_table->setCellWidget(rowCount, 0, channel.name);
    m_table->setCellWidget(rowCount, 1, channel.freq);
    m_table->setCellWidget(rowCount, 2, channel.modulation);
    m_table->setCellWidget(rowCount, 3, channel.ctcss);
    m_table->setCellWidget(rowCount, 4, channel.lockout);
    m_table->setCellWidget(rowCount, 5, channel.delay);
    m_table->setCellWidget(rowCount, 6, channel.priority);

    m_table->resizeColumnsToContents();
}