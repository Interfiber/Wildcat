//
// Created by hstasonis on 12/31/25.
//

#pragma once
#include <QDialog>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QTextEdit>

/**
 * Import data from other formats
 */
class ImportWindow : public QDialog
{
    Q_OBJECT
public:
    ImportWindow();

    enum class ImportMode
    {
        PastedTable,
        CSV
    };

public slots:
    /**
     * Open the ImportWindow dialog if the pasted text is in a supported import format
     */
    void openFromPaste();

private:
    QVBoxLayout* m_layout;

    /// @brief How-to info at the top of the screen
    QLabel* m_information;

    QLabel* m_noFormatsError;

    // @brief  Seperator between the information and mode buttons
    QFrame* m_seperatorLine;

    /// @brief  Input data field
    QTextEdit* m_importDataEdit;

    // Import modes

    QHBoxLayout* m_modeButtonLayout;

    std::vector<QRadioButton*> m_radioButtons;

    // Import and cancel buttons

    QHBoxLayout *m_bottomButtonLayout;

    QPushButton* m_importButton;
    QPushButton* m_cancelButton;
};
