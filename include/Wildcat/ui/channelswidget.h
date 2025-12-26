//
// Created by hstasonis on 12/26/25.
//

#pragma once
#include <QVBoxLayout>
#include <QTableWidget>
#include <QLabel>
#include <QComboBox>

/**
 * List of available channels
 */
class ChannelsWidget : public QWidget
{
   Q_OBJECT
public:
   explicit ChannelsWidget(QWidget* parent = nullptr);

   /// @brief  All components of a single channel displayed in the UI
   struct UIChannel
   {
      QLineEdit* name;

      QLineEdit* freq;
      QComboBox* modulation;
      QComboBox* ctcss;

      QComboBox* lockout;
      QComboBox* delay;
      QComboBox *priority;
   };

signals:
   void deviceConnectionChanged();

public slots:
   void addChannel() const;

private:
   QVBoxLayout *m_layout;
   QTableWidget *m_table;

   QLabel *m_noDevice;
};