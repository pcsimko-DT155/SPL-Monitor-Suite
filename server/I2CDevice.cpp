#include "I2CDevice.hpp"

#include <stdexcept>
#include <filesystem>
#include <filesystem>
#include <iostream>
#include <unistd.h>

#include <linux/i2c-dev.h>
#ifndef I2C_M_RD
#include <linux/i2c.h>
#endif
#include <sys/ioctl.h>

I2CDevice::I2CDevice(const std::string& device,
                     uchar i2c_address) :
  i2c_fid_( -1 ),
  i2c_address_(i2c_address)
{
  if ((i2c_fid_ = open(device.c_str(), O_RDWR)) < 0) {
    if (errno == ENOENT) {
      auto errCode = std::make_error_code(std::errc::no_such_file_or_directory);
      std::string err{"Error opening " + device};
      throw std::filesystem::filesystem_error(err, errCode);
    }
    else {
      throw std::runtime_error("Unknown error");
    }
  }
}

I2CDevice::~I2CDevice() {
  if (i2c_fid_) {
    close(i2c_fid_);
  }
}

int
I2CDevice::write(uchar reg, uchar data)
{
  uchar outbuf[] = {reg, data};

  struct i2c_msg msgs[1];
  struct i2c_rdwr_ioctl_data msgset[1];

  /* {addr, flags, len, buf} */
  msgs[0] = {i2c_address_, 0, 2, outbuf};

  /* {msgs, nmsgs} */
  msgset[0] = {msgs, 1};

  if (ioctl(i2c_fid_, I2C_RDWR, &msgset) < 0) {
    std::cerr << "ioctl(I2C_RDWR) in I2CDevice::write" << std::endl;
    return -1;
  }
  
  return 0;
}

int
I2CDevice::read(uchar reg, uchar* result)
{
  uchar outbuf[] = { reg };
  uchar inbuf[] = { 0 };

  struct i2c_msg msgs[2];
  struct i2c_rdwr_ioctl_data msgset[1];

  /* {addr, flags, len, buf} */
  msgs[0] = {i2c_address_, 0, 1, outbuf};
  msgs[1] = {i2c_address_, I2C_M_RD | I2C_M_NOSTART, 1, inbuf};

  /* {msgs, nmsgs} */
  msgset[0] = {msgs, 2};

  *result = 0;

  if (ioctl(i2c_fid_, I2C_RDWR, &msgset) < 0) {
    std::cerr << "ioctl(I2C_RDWR) in I2CDevice::read" << std::endl;
    return -1;
  }

  *result = inbuf[0];
  return 0;
}
