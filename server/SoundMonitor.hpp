#ifndef SOUNDMONITOR_HPP
#define SOUNDMONITOR_HPP

#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <deque>

static const size_t kDefaultQueueSize = 10;
static const std::chrono::milliseconds kDefaultSamplePeriod{100};

class I2CDevice;

class SoundMonitor {
  struct SplQueue {
    SplQueue() : qsz_(kDefaultQueueSize) {}

    void add(char data) {
      if (queue_.size() == qsz_) {
        queue_.pop_front();
      }
      
      queue_.push_back(data);
    }

    float average() const;

  private:  
    const size_t qsz_;
    std::deque<char> queue_;
  };
  
public:
  SoundMonitor(std::chrono::milliseconds samplePeriod = kDefaultSamplePeriod,
               const std::string& device="/dev/i2c-1",
               unsigned char i2cAddress = 0x48,
               bool freqAnalysisOn = false);
  ~SoundMonitor();

  bool configTavg(std::chrono::milliseconds tavg);
  void stop();
  void monitor();

  float spl() const { return splQueue_.average(); }
  
private:
  int file_;
  bool ready_;
  bool done_;
  std::atomic_char spl_;
  std::atomic<float> fspl_;
  SplQueue splQueue_;
  std::chrono::milliseconds samplePeriod_;
  bool freqAnalysisOn_;
  std::unique_ptr<I2CDevice> dB_meter_;
};

#endif // SOUNDMONITOR_HPP
