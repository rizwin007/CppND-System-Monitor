#include <string>
#include <chrono>
#include <iomanip>      // std::setfill

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  std::chrono::seconds totalSeconds{seconds};

  // return std::chrono::format("%T", seconds); // in C++20 :-)

  std::chrono::hours hours =
      std::chrono::duration_cast<std::chrono::hours>(totalSeconds);

  totalSeconds -= std::chrono::duration_cast<std::chrono::seconds>(hours);

  std::chrono::minutes minutes =
      std::chrono::duration_cast<std::chrono::minutes>(totalSeconds);

  totalSeconds -= std::chrono::duration_cast<std::chrono::seconds>(minutes);

  std::stringstream stream{};

  stream << std::setw(2) << std::setfill('0') << hours.count()      // HH
     << std::setw(1) << ":"                                         // :
     << std::setw(2) << std::setfill('0') << minutes.count()        // MM
     << std::setw(1) << ":"                                         // :
     << std::setw(2) << std::setfill('0') << totalSeconds.count();  // SS

  return stream.str();
}