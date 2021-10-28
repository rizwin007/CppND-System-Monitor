#include "linux_parser.h"
#include "processor.h"

// TODO: Return the aggregate CPU utilization
// https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
float Processor::Utilization() { 

    long idle_jiffies = LinuxParser::IdleJiffies();
    long total_jiffies = LinuxParser::Jiffies();

    long total_diff = total_jiffies - m_total_jiffies;
    long idle_diff = idle_jiffies - m_idle_jiffies;

    float util = 0.0f;
    try {
         util = ((float) (total_diff - idle_diff) )/ (float) (total_diff);
    } catch(...) {
        util = 0.0;
    }

    m_idle_jiffies = idle_jiffies;
    m_total_jiffies = total_jiffies;

    return util; 
}