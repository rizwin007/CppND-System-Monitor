#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

// Since not using variable "count" 
//https://stackoverflow.com/questions/11271889/global-variable-count-ambiguous
using namespace std;

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
vector<Process>& System::Processes() { 

    vector<int> pids = LinuxParser::Pids();

    processes_.clear();

    for(const int & pid: pids) {
        if((LinuxParser::Ram(pid) != "0.00")) {
            Process p(pid);
            float cpuUtil = p.CpuUtilization() * 100;

            if(cpuUtil > 0) {
                processes_.emplace_back(p);
            }
        }
    }

    std::sort(processes_.begin(), processes_.end());

    return processes_; 
}

// Return the system's kernel identifier (string)
std::string System::Kernel() { 
    return LinuxParser::Kernel();
}

// Return the system's memory utilization
float System::MemoryUtilization() { 
    return LinuxParser::MemoryUtilization(); 
}

// Return the operating system name
std::string System::OperatingSystem() { 
    return LinuxParser::OperatingSystem(); 
}

// Return the number of processes actively running on the system
int System::RunningProcesses() { 
    return LinuxParser::RunningProcesses(); 
}

// Return the total number of processes on the system
int System::TotalProcesses() { 
    return LinuxParser::TotalProcesses(); 
}

// Return the number of seconds since the system started running
long int System::UpTime() { 
    return LinuxParser::UpTime(); 
}