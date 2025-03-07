#include "SoundMonitor.hpp"

#include <fcntl.h>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#include <thread>

float
SoundMonitor::SplQueue::average() const
{
  float sum = 0.0;

  if (queue_.size() == 0) return sum;

  for (const auto& d : queue_) {
    sum += static_cast<float>(d);
  }

  return sum / queue_.size();
}

SoundMonitor::SoundMonitor(std::chrono::milliseconds samplePeriod,
                           const std::string& device,
                           int i2cAddress) :
  i2cAddress_(i2cAddress),
  ready_(true),
  done_(false),
  samplePeriod_(samplePeriod)
{
  file_ = open(device.c_str(), O_RDWR);
  if (file_ < 0)
  {
    std::cerr << "Failed to open I2C1!" << std::endl;
    ready_ = false;
  }

  if (ioctl(file_, I2C_SLAVE, i2cAddress) < 0)
  {
    std::cerr << "dB Meter is not connected/recognized at I2C addr = " << i2cAddress
              << std::endl;
    close(file_);
    ready_ = false;
  }
}

SoundMonitor::~SoundMonitor()
{
  close(file_);
}

void
SoundMonitor::stop()
{
  done_ = true;
}

void
SoundMonitor::monitor()
{
  char data;

  while (not done_) {
    data = 0x0A;
    if (write(file_, &data, 1) != 1) {
      std::cerr << "Failed to write register " << data << std::endl;
      break;
    }

    if (read(file_, &data, 1) != 1) {
      std::cerr << "Failed to read register " << data << std::endl;
      break;
    }

    splQueue_.add(data);
    std::this_thread::sleep_for(samplePeriod_);
  }

  close(file_);
}
