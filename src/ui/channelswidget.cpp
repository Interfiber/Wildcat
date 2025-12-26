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

    const int rowCount = m_table->rowCount() - 1;

    UIChannel channel{};
    channel.channel = WildcatMainWindow::get()->m_device->newChannel();

    // Name

    channel.name = new QLineEdit(nullptr);
    channel.name->setPlaceholderText("Channel name");

    // Frequency

    channel.freq = new QLineEdit(nullptr);
    channel.freq->setPlaceholderText("Frequency (MHz)");

    // Modulation

    channel.modulation = new QComboBox(nullptr);
    channel.modulation->addItems({
    "Automatic","AM", "FM", "NFM"
    });

    channel.ctcss = new QComboBox(nullptr);
    channel.ctcss->addItems(Wildcat_GetCTCSSCodes());
    channel.ctcss->setMinimumContentsLength(14);

    // Lockout

    channel.lockout = new QComboBox(nullptr);
    channel.lockout->addItems({
    "Off", "Lockout"
    });

    // Delay

    channel.delay = new QComboBox(nullptr);
    channel.delay->addItems(WildcatChannel::DELAY_VALUES);
    channel.delay->setCurrentIndex(WildcatChannel::DELAY_VALUES.indexOf("2"));

    // Priority

    channel.priority = new QComboBox(nullptr);
    channel.priority->addItems({ "Off", "PCH" });

    m_channels.push_back(channel);

    // Insert channel into the UI
    m_table->setCellWidget(rowCount, 0, channel.name);
    m_table->setCellWidget(rowCount, 1, channel.freq);
    m_table->setCellWidget(rowCount, 2, channel.modulation);
    m_table->setCellWidget(rowCount, 3, channel.ctcss);
    m_table->setCellWidget(rowCount, 4, channel.lockout);
    m_table->setCellWidget(rowCount, 5, channel.delay);
    m_table->setCellWidget(rowCount, 6, channel.priority);
}
