/*
 * Based on James Dunne's (github JamesDunne) C library for reading/writing I2C slave device registers
 */
#include <fcntl.h>
#include <string>

class I2CDevice {
  using uchar = unsigned char;
  
public:
  I2CDevice(const std::string& device,
            uchar i2c_address);
  ~I2CDevice();

  int write(uchar reg, uchar data);
  int read(uchar reg, uchar* result);
  
private:
  int i2c_fid_;
  uchar i2c_address_; /* slave address */
};
