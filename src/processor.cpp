#include "linux_parser.h"
#include "processor.h"

// Return the aggregate CPU utilization
// https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
float Processor::Utilization() { 

    long idleJiffies = LinuxParser::IdleJiffies();
    long totalJiffies = LinuxParser::Jiffies();

    long totalDiff = totalJiffies - m_totalJiffies;
    long idleDiff = idleJiffies - m_idleJiffies;

    float util = 0.0f;
    try {
         util = (static_cast<float>(totalDiff - idleDiff) )/ (static_cast<float>(totalDiff));
    } 
    catch(...) {
        util = 0.0;
    }

    m_idleJiffies = idleJiffies;
    m_totalJiffies = totalJiffies;

    return util; 
}