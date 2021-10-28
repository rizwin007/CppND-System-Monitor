#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds[[maybe_unused]]) { 

    long hr, min, sec, min_in_sec;
    string s_hr, s_min, s_sec, final_string;

    hr = seconds / 3600;
    min_in_sec = seconds % 3600;

    min = min_in_sec / 60;

    sec = min_in_sec % 60;

    if(hr < 10) {
        s_hr = "0" + std::to_string(hr);
    }
    else {
        s_hr = std::to_string(hr);
    }

    if(min < 10) {
        s_min = "0" + std::to_string(min);
    }
    else {
        s_min = std::to_string(min);
    }

    if(sec < 10) {
        s_sec = "0" + std::to_string(sec);
    }
    else {
        s_sec = std::to_string(sec);
    }

    final_string = s_hr + ":" + s_min + ":" + s_sec;

    return final_string; 
}