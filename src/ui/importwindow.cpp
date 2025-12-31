//
// Created by hstasonis on 12/31/25.
//

#include <QClipboard>
#include <qguiapplication.h>
#include <QLineEdit>
#include <QPushButton>
#include <Wildcat/ui/importwindow.h>

#include "Wildcat/fs/archive.h"

ImportWindow::ImportWindow()
{
    m_information = new QLabel("Select a format to import.\nImporting will override any data which is in conflict with the new version");

    for (const auto &pair : WildcatArchiveImporter::get()->getArchivePairs())
    {
        m_radioButtons.push_back(new QRadioButton(pair.archive->getArchiveName().data()));
    }

    if (m_radioButtons.empty())
        m_noFormatsError = new QLabel("No valid import formats registered!");

    m_seperatorLine = new QFrame();
    m_seperatorLine->setFrameStyle(QFrame::HLine);

    m_modeButtonLayout = new QHBoxLayout;

    for (auto &button : m_radioButtons)
        m_modeButtonLayout->addWidget(button);

    if (m_radioButtons.empty())
        m_modeButtonLayout->addWidget(m_noFormatsError);

    m_importDataEdit = new QTextEdit();
    m_importDataEdit->setPlaceholderText("Paste or enter data to import");

    m_importButton = new QPushButton("Confirm import");
    m_importButton->setIcon(QIcon::fromTheme(QIcon::ThemeIcon::FolderOpen));
    m_importButton->setDisabled(true);

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

void ImportWindow::openFromPaste()
{
    m_importDataEdit->setText(QGuiApplication::clipboard()->text());

    show();
}
