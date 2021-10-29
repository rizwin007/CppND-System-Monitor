#include <iostream>
#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid): m_pid(pid) {
    long seconds = LinuxParser::UpTime() - LinuxParser::UpTime(m_pid);
    try {
        m_util = ((float) LinuxParser::ActiveJiffies(m_pid)) / (float) seconds;
    } 
    catch (const std::exception& e) { // caught by reference to base
        std::cout << "Exception was caught in Process::Pid() with message '"<< e.what() << "'\n";
    }
}

// Return this process's ID
int Process::Pid() { 
    return m_pid; 
}

// Return this process's CPU utilization
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
float Process::CpuUtilization() { 
    long seconds = LinuxParser::UpTime() - LinuxParser::UpTime(Pid());
    try {
        m_util = ((float) LinuxParser::ActiveJiffies(Pid())) / (float) seconds;
    } 
    catch (const std::exception& e) { // caught by reference to base
        std::cout << "Exception was caught in Process::Pid() with message '"<< e.what() << "'\n";
    }

    return m_util; 
}

// Return the command that generated this process
string Process::Command() {
  string cmd = LinuxParser::Command(Pid());
  const int width = 50;

  if (cmd.length() > width)
    cmd = cmd.substr(0, width) + "...";

  return cmd; 
}

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(Pid()); }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(Pid()); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(Pid()); }

// Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
    return m_util < a.m_util;
}
