#ifndef WILDCATUSERDRIVER_H
#define WILDCATUSERDRIVER_H

/**
 * A usermode driver for the wildcat BC125AT serial port
 * Adapted from: https://github.com/itsmaxymoo/bc125py/blob/main/bc125py/con.py
 */
class WildcatUserDriver {
public:
  WildcatUserDriver(bool run = false);
};

#endif // WILDCATUSERDRIVER_H
