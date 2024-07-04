#ifndef WILDCATCHANNEL_H
#define WILDCATCHANNEL_H

#include <string>

class WildcatChannel {
public:
  WildcatChannel();

  enum class Modulation { Auto, Am, Fm, Nfm };

  enum class LockoutMode {
    Off,

    /// L/O
    LO
  };

  enum class Priority {
    Off,

    /// P-Ch
    PCH
  };

  std::string name = "NO NAME";

  float frequency = 0.0f;

  Modulation mod = Modulation::Auto;

  std::string ctcss = "...";

  LockoutMode lockout = LockoutMode::Off;

  int delay = 2;

  Priority priority = Priority::Off;

  struct {
    int modSelected = 0;
    int lockoutSelected = 0;
    int prioritySelected = 0;
  } internalState;
};

#endif // WILDCATCHANNEL_H
