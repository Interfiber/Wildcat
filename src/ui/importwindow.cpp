//
// Created by hstasonis on 12/31/25.
//

#include <QClipboard>
#include <qguiapplication.h>
#include <QMessageBox>
#include <QLineEdit>
#include <QPushButton>
#include <Wildcat/ui/importwindow.h>
#include <Wildcat/ui/mainwindow.h>

#include "Wildcat/fs/archive.h"
#include "Wildcat/global.h"
#include "Wildcat/ui/channelswidget.h"

ImportWindow::ImportWindow()
{
    m_information = new QLabel("Select a format to import.\nAll the imported channels will be appended to the current bank");

    for (const auto &pair : WildcatArchiveImporter::get()->getArchivePairs())
    {
        QRadioButton* button = new QRadioButton(pair.archive->getArchiveName().data());

        connect(button, &QRadioButton::clicked, this, [pair, this] (bool checked)
        {
            m_importButton->setDisabled(false);
        });

        m_radioButtons.push_back(button);
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

    connect(m_importButton, &QPushButton::clicked, this, [this] ()
    {
        if (DEVICE == nullptr)
        {
            QMessageBox::warning(this, "Wildcat importer", "A device must be connected in order to import data!");

            return;
        }

        // Find correct archive to invoke based off the checked radio button
        for (int i = 0; i < WildcatArchiveImporter::get()->getArchivePairs().size(); i++)
        {
            auto pair = WildcatArchiveImporter::get()->getArchivePairs()[i];

            // Radio button is checked, import the archive
            if (m_radioButtons[i]->isChecked())
            {
                pair.archive->importArchive(m_importDataEdit->toPlainText().toStdString());

                // Close the window now

                QMessageBox::information(this, "Wildcat importer", "The import operation has completed!");

                accept();
                break;
            }
        }
    });

    m_cancelButton = new QPushButton("Cancel");

    connect(m_cancelButton, &QPushButton::clicked, this, &ImportWindow::reject);

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

void ImportWindow::openFromPaste(ChannelsWidget *channelsWidget)
{
    const QString clipboard = QGuiApplication::clipboard()->text();

    m_importDataEdit->setText(clipboard);

    // Determine the correct archive type radio button to select

    for (int i = 0; i < WildcatArchiveImporter::get()->getArchivePairs().size(); i++)
    {
        auto pair = WildcatArchiveImporter::get()->getArchivePairs()[i];

        try {
                // Check if the pasted text is this archive type
            if (std::invoke(pair.check, pair.archive, clipboard.toStdString()))
            {
                    m_radioButtons[i]->setChecked(true);
                    m_importButton->setDisabled(false); // Importing is now possible!
                    break;
            }
        } catch (std::exception &e)
        {
            // Somebody inserted some really fucked up input data
            QMessageBox::warning(this, "Wildcat Importer", ("Error during archive validation: " + std::string(e.what())).data());
        }
    }

    // Use exec() so the user can't interact with the parent window
    exec();
}
