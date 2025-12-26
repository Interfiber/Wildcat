//
// Created by hstasonis on 12/26/25.
//

#include <Wildcat/ui/channelswidget.h>

#include "../../include/Wildcat/io/device.h"

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

void ChannelsWidget::addChannel() const
{
    m_table->setRowCount(m_table->rowCount() + 1);

    int rowCount = m_table->rowCount();
}
