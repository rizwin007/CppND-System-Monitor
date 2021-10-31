#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <iomanip>  // std::setprecision
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::stol;
using std::string;
using std::to_string;
using std::vector;

/************************ local functions ***********************************/
template <typename T>
T findValueByKey(std::string const &keyFilter, std::string const &filename) {
  std::string line, key;
  T value;

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == keyFilter) {
          return value;
        }
      }
    }
    stream.close();
  }

  return value;
};

template <typename T>
T getValueOfFile(std::string const &filename) {
  std::string line;
  T value;

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
    stream.close();
  }

  return value;
};

/************************ public functions ***********************************/
// An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }  
    filestream.close();
  }

  return value;
}

// An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
    stream.close();
  }
  
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.emplace_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line;
  string key;
  string value;
  string unit;
  float util = 0.0f;

  // reading "/proc/memInfo"
  float memTotal = findValueByKey<float>(filterMemTotalString, kMeminfoFilename);
  float memFree = findValueByKey<float>(filterMemFreeString, kMeminfoFilename);

  try {
    util = (memTotal - memFree)/memTotal;
  } 
  catch (const std::exception& e) { // caught by reference to base
      std::cout << "Exception was caught in LinuxParser::MemoryUtilization() , with message '"
                << e.what() << "'\n";
  }

  return util;
}

// Read and return the system uptime
long int LinuxParser::UpTime() { 
  string line;
  long int value =  getValueOfFile<long int>(kUptimeFilename);
  return value;
 }

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies() ; 
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  string line;
  string sValue;
  long totalTime = 0;

  vector <string> values;

  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);

    while(linestream >> sValue) {
      values.emplace_back(sValue);
    }

    try {
      totalTime = stol(values[ProcessTime::kUtime_]) +  stol(values[ProcessTime::kStime_]) +  
                  stol(values[ProcessTime::kCutime_]) +  stol(values[ProcessTime::kCstime_]);
    }
    catch (const std::exception& e) { // caught by reference to base
      std::cout << "Exception was caught in LinuxParser::ActiveJiffies(pid) with message '"
                << e.what() << "'\n";
    }
    filestream.close();
  }

  return totalTime / sysconf(_SC_CLK_TCK);
}

// Read and return the number of active jiffies for the system
// based on https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
long LinuxParser::ActiveJiffies() { 
  vector<string> jiffies = CpuUtilization();

  return stol(jiffies[CPUStates::kUser_]) + stol(jiffies[CPUStates::kNice_]) + 
         stol(jiffies[CPUStates::kSystem_]) + stol(jiffies[CPUStates::kIRQ_]) + 
         stol(jiffies[CPUStates::kSoftIRQ_]) + stol(jiffies[CPUStates::kSteal_]);

}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  
  vector<string> jiffies = CpuUtilization();

  return stol(jiffies[CPUStates::kIdle_]) + stol(jiffies[CPUStates::kIOwait_]);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string line, cpu, jiffy;
  vector<string> jiffies;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);

    linestream >> cpu;
    while(linestream >> jiffy) {
      jiffies.emplace_back(jiffy);
    }
    filestream.close();
  }
  
  return jiffies;
 }

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  string key;
  string value;

  int totalprocesses = findValueByKey<int>(filterProcesses, kStatFilename);

  return totalprocesses;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line;
  string key;
  string value;
  int running_processes = findValueByKey<int>(filterRunningProcesses, kStatFilename);

  return running_processes;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  return string(getValueOfFile<string>(std::to_string(pid) + kCmdlineFilename)); 
}

// Read and return the memory used by a process
// https://stackoverflow.com/questions/29200635/convert-float-to-string-with-precision-number-of-decimal-digits-specified
string LinuxParser::Ram(int pid) { 
  string line;
  string key;

  float ram = findValueByKey<float>(filterProcMem, std::to_string(pid) + kStatusFilename);

  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << (ram/1024.0f);
  string ramInMB = stream.str();

  return ramInMB;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string line;
  string key;
  string value = findValueByKey<string>(filterUID, std::to_string(pid) + kStatusFilename);

  return value; 
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  
  string line;
  string key;
  string user, pwd, uid;
  bool exit = false;

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> user >> pwd >> uid) {
        if(uid == Uid(pid)) {
          exit = true; //enable condition to exit outer while()
          break;
        }
      }
      if(exit) break;
    }  
    filestream.close();
  }

  return user; 
 }

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
 string line;
 string sValue;
 long uptime = 0;

  vector <string> values;

  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);

    while(linestream >> sValue) {
        values.emplace_back(sValue);
    }
    filestream.close();
  }
  

  try { 
    if(!values.empty()) { // prevent accessing invalid memory
      // index 21 is starttime  
      uptime = stol(values[21]) / sysconf(_SC_CLK_TCK);
    }
  } 
  catch (const std::exception& e) { // caught by reference to base
    std::cout << "Exception was caught in LinuxParser::UpTime(pid) with message '"
              << e.what() << "'\n";
  }
  
  return uptime;
}
