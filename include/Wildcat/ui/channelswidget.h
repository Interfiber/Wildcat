//
// Created by hstasonis on 12/26/25.
//

#pragma once
#include <QVBoxLayout>
#include <QTableWidget>
#include <QLabel>
#include <QComboBox>

class QCheckBox;
class QPushButton;
class WildcatChannel;
/**
 * List of available channels
 */
class ChannelsWidget : public QWidget
{
   Q_OBJECT
public:
   explicit ChannelsWidget(QWidget* parent = nullptr);
   ~ChannelsWidget() override;

   /// @brief  All components of a single channel displayed in the UI
   struct UIChannel
   {
      std::shared_ptr<WildcatChannel> channel;

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
   void addChannel();

private:
   QHBoxLayout *m_layout;
   QVBoxLayout *m_quickActionsLayout;

   QWidget* m_quickActions;

   QLabel* m_quickActionsLabel;

   QPushButton *m_writeToDevice;
   QCheckBox *m_enableHotload;

   std::vector<QTableWidget*> m_banks;

   QTabWidget *m_tabWidget;

   QLabel *m_noDevice;

   std::vector<UIChannel> m_channels;
};