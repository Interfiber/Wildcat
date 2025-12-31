//
// Created by hstasonis on 12/31/25.
//

#include <QLineEdit>
#include <QPushButton>
#include <Wildcat/ui/importwindow.h>

ImportWindow::ImportWindow()
{
    m_information = new QLabel("Select a format to import.\nImporting will override any data which is in conflict with the new version");

    m_tableImport = new QRadioButton("Tab Seperated Table");
    m_csvImport = new QRadioButton("Comma Separated Values (CSV)");

    m_seperatorLine = new QFrame();
    m_seperatorLine->setFrameStyle(QFrame::HLine);

    m_modeButtonLayout = new QHBoxLayout;
    m_modeButtonLayout->addWidget(m_tableImport);
    m_modeButtonLayout->addWidget(m_csvImport);

    m_importDataEdit = new QTextEdit();
    m_importDataEdit->setPlaceholderText("Paste or enter data to import");


    m_importButton = new QPushButton("Confirm import");
    m_importButton->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::FolderOpen));

    m_cancelButton = new QPushButton("Cancel");

    m_bottomButtonLayout = new QHBoxLayout;
    m_bottomButtonLayout->addWidget(m_importButton);
    m_bottomButtonLayout->addWidget(m_cancelButton);

    m_layout = new QVBoxLayout;
    m_layout->addWidget(m_information);
    m_layout->addWidget(m_seperatorLine);
    m_layout->addLayout(m_modeButtonLayout);
    m_layout->addWidget(m_importDataEdit);
    m_layout->addLayout(m_bottomButtonLayout);

    setLayout(m_layout);

    setWindowTitle("Quick import wizard");
    setModal(true);
}