//
// Created by hstasonis on 1/3/26
//

#pragma once
#include <QObject>
#include <memory>

class WildcatDevice;


/**
 * @brief Global state for non-UI related objects such as the WildcatDevice pointer
 */
class WildcatGlobalState : public QObject
{
  Q_OBJECT
public:
  WildcatGlobalState() = default;

  [[nodiscard]] static std::shared_ptr<WildcatGlobalState>
  get()
  {
    static std::shared_ptr<WildcatGlobalState> state = std::make_shared<WildcatGlobalState>();

    return state;
  }

  /// @brief  Serial device
  std::shared_ptr<WildcatDevice> device = nullptr;

  /// @brief  Enable channel hotloading?
  bool hotload = false;

  /// @brief  Index of the currently selected bank
  int currentBankIndex = 1;

signals:
  void showWarning(const std::string& message);
};


#define DEVICE WildcatGlobalState::get()->device
#define HOTLOAD WildcatGlobalState::get()->hotload
