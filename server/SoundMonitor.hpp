#ifndef SOUNDMONITOR_HPP
#define SOUNDMONITOR_HPP

#include <atomic>
#include <chrono>
#include <string>
#include <deque>

static const size_t kDefaultQueueSize = 10;
static const std::chrono::milliseconds kDefaultSamplePeriod{100};

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
               int i2cAddress = 0x48);
  ~SoundMonitor();

  void stop();
  void monitor();

  float spl() const { return splQueue_.average(); }
  
private:
  int file_;
  int i2cAddress_;
  bool ready_;
  bool done_;
  std::atomic_char spl_;
  std::atomic<float> fspl_;
  SplQueue splQueue_;
  std::chrono::milliseconds samplePeriod_;
};

#endif // SOUNDMONITOR_HPP
