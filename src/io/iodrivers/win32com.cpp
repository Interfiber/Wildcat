//
// Created by hstasonis on 12/29/25.
//

#include <QDebug>
#include <Wildcat/io/iodrivers/win32com.h>
#include <iostream>
#include <thread>
#include <winbase.h>

#ifndef PATH_MAX
#define PATH_MAX 255
#endif

std::string
WildcatUtil_GetLastError()
{
  DWORD errorMessageID = ::GetLastError();
  if (errorMessageID == 0)
  {
    return "NoError"; // No error message has been recorded
  }

  LPSTR messageBuffer = nullptr;
  size_t size
    = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                     errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

  std::string message(messageBuffer, size);
  LocalFree(messageBuffer);

  return message;
}

std::vector<std::string>
WildcatWin32ComDriver::getConnectedDevices()
{
  // Query all avalible COM ports on this device

  std::vector<std::string> comPorts{};

  char comPath[PATH_MAX];

  for (uint32_t port = 0; port < MAX_PORTS; port++)
  {
    std::string portName = COM_PREFIX + std::to_string(port);

    DWORD dosDev = QueryDosDeviceA(portName.c_str(), comPath, PATH_MAX);

    // No device found with query
    if (dosDev == 0)
      continue;

    comPorts.push_back(portName);
  }

  return comPorts;
}

void
WildcatWin32ComDriver::releaseDevice()
{
  CloseHandle(m_device);

  m_device = nullptr;
}

bool
WildcatWin32ComDriver::isConnected()
{
  return m_device != nullptr;
}

WildcatIODriver::IOResult
WildcatWin32ComDriver::readFromDevice()
{
  std::string buffer;

  char tmp = '\0';

  while (ReadFile(m_device, &tmp, sizeof(tmp), nullptr, nullptr) > 0)
  {
    if (tmp == '\r' || tmp == '\n')
      break;

    buffer += tmp;
  }

  printf("Win32Com Driver: ReadFile error = %s\n", WildcatUtil_GetLastError().c_str());
  printf("Win32Com Driver: Read %d bytes from device\n", buffer.size());

  if (buffer.back() == '\n')
    buffer.pop_back();

  if (buffer.back() == '\r')
    buffer.pop_back();

  if (buffer.empty())
  {
    printf("Win32Com Driver: Got empty buffer, trying again for readFromDevice()\n");

    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    return readFromDevice(); // Try again.
  }

  printf("Win32Com Driver: Read from device: %s\n", buffer.c_str());

  return WildcatIODriver::IOResult(buffer, false);
}

WildcatIODriver::IOResult
WildcatWin32ComDriver::writeToDevice(const std::string& buffer)
{
  printf("Win32Com Driver: Writing to device: %s", buffer.c_str());

  DWORD bytesWritten = 0;

  auto writeStatus = WriteFile(m_device, buffer.data(), buffer.size(), &bytesWritten, nullptr);

  if (writeStatus == FALSE)
  {
    return WildcatIODriver::IOResult("Failed to write to COM device: " + WildcatUtil_GetLastError(), true);
  }

  printf("Win32Com Driver: Wrote %d bytes to serial device\n", bytesWritten);

  return WildcatIODriver::IOResult("Wrote to COM device", false);
}

WildcatIODriver::IOResult
WildcatWin32ComDriver::connectToDevice(const std::string& name)
{
  printf("Win32Com Driver: Connecting to device at: %s\n", name.c_str());

  m_device
    = CreateFileA(("\\\\.\\" + name).c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);

  if (m_device == INVALID_HANDLE_VALUE)
  {
    return WildcatIODriver::IOResult(
      "Failed to open " + name + " for GENERIC_READ and GENERIC_WRITE: " + WildcatUtil_GetLastError(), true);
  }

  // Setup serial device

  COMMTIMEOUTS timeouts{};
  DCB dcbSerial{};

  // DCB Serial
  dcbSerial.DCBlength = sizeof(dcbSerial);
  dcbSerial.BaudRate = CBR_9600;
  dcbSerial.ByteSize = 8;
  dcbSerial.StopBits = TWOSTOPBITS;
  dcbSerial.Parity = NOPARITY;

  if (SetCommState(m_device, &dcbSerial) == FALSE)
  {
    return WildcatIODriver::IOResult("Failed to call SetCommState for COM device: " + WildcatUtil_GetLastError(), true);
  }

  // Timeouts
  timeouts.ReadIntervalTimeout = 50;
  timeouts.ReadTotalTimeoutConstant = 50;
  timeouts.ReadTotalTimeoutMultiplier = 10;
  timeouts.WriteTotalTimeoutConstant = 50;
  timeouts.WriteTotalTimeoutMultiplier = 10;

  if (SetCommTimeouts(m_device, &timeouts) == FALSE)
  {
    return WildcatIODriver::IOResult("Failed to call SetCommTimeouts for COM device: " + WildcatUtil_GetLastError(),
                                     true);
  }

  return WildcatIODriver::IOResult("Connected to device", false);
}
