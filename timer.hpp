#include <chrono>

namespace mytimer {
  class timer {
  private:
    std::chrono::system_clock::time_point startTime;
    inline std::chrono::system_clock::time_point getTime() {
      return std::chrono::system_clock::now();
    }
  public:
    inline void set() {
      startTime = getTime();
    }
    inline double getMsec() {
      auto dur = getTime() - startTime;
      auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
      return msec;
    }
    inline double getSec() {
      return (double)getMsec()/1000.0;
    }
    inline double get() {
      return getSec();
    }
    inline void print(const std::string str= "") {
      double t = get();
      if(t > 1) std::cout << str << " " << t << " sec." << std::endl;
      else    std::cout << str << " " << t*1000.0 << "msec." << std::endl;
    }
    timer() {
      set();
    }
  };
};
