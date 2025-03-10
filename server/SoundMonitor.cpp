#include "SoundMonitor.hpp"
#include "I2CDevice.hpp"

#include <fcntl.h>
#include <iostream>
#include <time.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>

#include <thread>

#define TAVG_HI 0x07
#define TAVG_LO 0x08
#define DECIBEL 0x0A

static const unsigned char kTavgHiRegAddr = TAVG_HI;
static const unsigned char kTavgLoRegAddr = TAVG_LO;
static const unsigned char kDbRegAddr     = DECIBEL;

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
                           unsigned char i2cAddress,
                           bool freqAnalysisOn) :
  ready_(true),
  done_(false),
  samplePeriod_(samplePeriod),
  freqAnalysisOn_(freqAnalysisOn),
  dB_meter_( std::make_unique<I2CDevice>(device, i2cAddress))
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

bool
SoundMonitor::configTavg(std::chrono::milliseconds tavg)
{
  if (tavg.count() > 1000) {
    std::cerr << "tavg greater than supported max value" << std::endl;
    return false;
  }

  if (tavg.count() < 125) {
    std::cerr << "tavg less than supported min value" << std::endl;
    return false;
  }
  
  auto t = static_cast<uint16_t>(tavg.count());
  
  u_char hiByte = (t >> 8) & 0xFF;
  u_char loByte = t & 0xFF;

  if (dB_meter_->write(kTavgHiRegAddr, hiByte) < 0) {
    std::cerr << "Failed to write TAVG_HI register" << std::endl;
    return false;
  }
  if (dB_meter_->write(kTavgLoRegAddr, loByte) < 0) {  
    std::cerr << "Failed to write TAVG_LO register" << std::endl;
    return false;
  }

  return true;
  
}

void
SoundMonitor::monitor()
{
  char data;
  unsigned char db;
  
  while (not done_) {

    if (dB_meter_->read(kDbRegAddr, &db) < 0) {
      std::cerr << "Failed to read DECIBEL register" << std::endl;
    }
    splQueue_.add(db);    

    if (freqAnalysisOn_) {
      // TODO: fill the spectrum vector
    }
    std::this_thread::sleep_for(samplePeriod_);
  }

  close(file_);
}
