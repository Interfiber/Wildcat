//
// Created by hstasonis on 12/30/25.
//

#pragma once
#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

/**
 * Popup dialog to enter text
 */
class TextInputDialog : public QDialog
{
public:
  explicit TextInputDialog(const std::string& title, QWidget* parent = nullptr);
  ~TextInputDialog() override;

  /// @brief  Get the text entered after the dialog is accepted
  [[nodiscard]] std::string
  getResult() const
  {
    return m_result;
  }

private:
  QVBoxLayout* m_layout;
  QHBoxLayout* m_buttonLayout;

  QLabel* m_title;
  QLineEdit* m_edit;

  QPushButton* m_ok;
  QPushButton* m_cancel;

  std::string m_result;
};
