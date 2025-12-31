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

private:
    QVBoxLayout* m_layout;

    /// @brief How-to info at the top of the screen
    QLabel* m_information;

    // @brief  Seperator between the information and mode buttons
    QFrame* m_seperatorLine;

    /// @brief  Input data field
    QTextEdit* m_importDataEdit;

    // Import modes

    QHBoxLayout* m_modeButtonLayout;
    QRadioButton* m_tableImport;
    QRadioButton* m_csvImport;

    // Import and cancel buttons

    QHBoxLayout *m_bottomButtonLayout;

    QPushButton* m_importButton;
    QPushButton* m_cancelButton;
};
