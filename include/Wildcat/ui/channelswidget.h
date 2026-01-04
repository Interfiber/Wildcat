//
// Created by hstasonis on 12/26/25.
//

#pragma once
#include <QVBoxLayout>
#include <QTableWidget>
#include <QLabel>
#include <QComboBox>
#include <QThread>
#include <QSettings>

class WildcatDevice;
class QCheckBox;
class QPushButton;
class WildcatChannel;

class BankLoaderThread : public QThread
{
   Q_OBJECT
public:
   BankLoaderThread(const std::shared_ptr<WildcatDevice> &device, int bank);

   void run() override;

signals:
   void requestNewChannel(const std::shared_ptr<WildcatChannel> &channel);

private:
   std::shared_ptr<WildcatDevice> m_device;
   int m_bank;
};

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
      void destroy() const;

      std::shared_ptr<WildcatChannel> channel;

      QLineEdit* name;

      QLineEdit* freq;
      QComboBox* modulation;
      QComboBox* ctcss;

      QComboBox* lockout;
      QComboBox* delay;
      QComboBox *priority;
   };

public slots:
   void addChannel(const std::shared_ptr<WildcatChannel> &channel = nullptr);
   void loadCurrentBank();
   void clearChannels();

private:
   QHBoxLayout *m_layout;
   QVBoxLayout *m_quickActionsLayout;

   QMenu* m_contextMenu;

   QAction* m_setBankNickname;

   QWidget* m_quickActions;

   QLabel* m_quickActionsLabel;

   QPushButton *m_writeToDevice;
   QCheckBox *m_enableHotload;

   std::vector<QTableWidget*> m_banks;

   QSettings m_settings;

   QTabWidget *m_tabWidget;

   std::vector<UIChannel> m_channels;
   std::unordered_map<int, bool> m_loadedBanks;
};
