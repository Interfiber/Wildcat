#ifndef SCANNER_H
#define SCANNER_H
#include <cstddef>
#include <termios.h>

/**
 * Number of channels the scanner has
 */
const int SCANNER_CHANNELS = 500;

/**
 * Transfer speed of the scanner
 */
const int SCANNER_SPEED = B115200;

/**
 * Used like:
 * usleep((charCount + 25) * SCANNER_TRANSFER_WAIT_TIME);
 */
const int SCANNER_TRANSFER_WAIT_TIME = 100;

/**
 * Size of the buffer we copy data from the scanner into
 */
const size_t SCANNER_TRANSFER_BUFFER_SIZE = 130;

#endif