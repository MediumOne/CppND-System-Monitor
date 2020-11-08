#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

/**
 *  
 * SYSTEM DATA methods.
 * 
 */

// DONE: An example of how to read data from the filesystem
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
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
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
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string label, memUnit;
  float memValue;
  vector<float> allMemValues;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);

  while (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> label >> memValue >> memUnit;

    allMemValues.emplace_back(memValue);

    // Exit when reaching third line
    if ((label.compare("MemAvailable:") == 0)) {
      break;
    }
  }
  stream.close();

  float memAvailable = allMemValues.back();
  allMemValues.pop_back();

  //We don't need MemFree
  allMemValues.pop_back(); 

  float memTotal = allMemValues.back();
  allMemValues.pop_back();

  return ((memTotal - memAvailable) / memTotal);
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  long uptime = 0, idleTime = 0;

  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idleTime;
  }
  stream.close();
  return uptime;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string name;
  int value;

  std::ifstream stream(kProcDirectory + kStatFilename);
  while (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> name >> value;

    if ((name.compare("processes") == 0)) {
      stream.close();
      return value;
    }
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::string line;
  std::string name;
  int value;

  std::ifstream stream(kProcDirectory + kStatFilename);
  while (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> name >> value;

    if ((name.compare("procs_running") == 0)) {
      stream.close();
      return value;
    }
  }
  return 0;
}

/**
 *  
 * PROCESSOR DATA methods.
 * 
 */

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
      std::string line;
      //The first line has the aggregate CPU data.
      std::getline(filestream, line);
      std::istringstream linestream(line);
      std::string cpu;
      CPUStates cpuStates{};
      linestream >> cpu >> cpuStates.user >> cpuStates.nice >> cpuStates.system >> cpuStates.idle >> cpuStates.iowait 
                        >> cpuStates.irq >> cpuStates.softirq >> cpuStates.steal >> cpuStates.guest >> cpuStates.guestnice;
      return cpuStates.Total();
  }
  return 0;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
      std::string line;
      //The first line has the aggregate CPU data.
      std::getline(filestream, line);
      std::istringstream linestream(line);
      std::string cpu;
      CPUStates cpuStates{};
      linestream >> cpu >> cpuStates.user >> cpuStates.nice >> cpuStates.system >> cpuStates.idle >> cpuStates.iowait 
                        >> cpuStates.irq >> cpuStates.softirq >> cpuStates.steal >> cpuStates.guest >> cpuStates.guestnice;
      return cpuStates.Active();
  }
  return 0;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  return Jiffies() - ActiveJiffies();
}

// Read and return CPU utilization
float LinuxParser::CpuUtilization() { 
  return (float)ActiveJiffies() / (float)Jiffies(); 
}

/**
 * 
 * PROCESS Level data methods
 * 
 */

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  std::stringstream filename;
  filename << kProcDirectory << "/" << pid << "/" << kStatFilename;
  std::ifstream filestream(filename.str());
  if (filestream.is_open()) {
      std::string line;
      std::getline(filestream, line);
      std::istringstream linestream(line);
      std::string ignore;
      long utime, stime, cutime, cstime, starttime;
      for(int i = 0; i < 13; i++) linestream >> ignore;
      linestream >> utime >> stime >> cutime >> cstime ;
      for(int i = 0; i < 4; i++) linestream >> ignore;
      linestream >> starttime;
      return utime + stime + cutime + cstime +starttime;
  }
  return 0; 
}

// TODO: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  std::string command;
  std::string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> command;
    stream.close();
    return command;
  }
  return "";
}

// TODO: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  std::string code, vmSize, idle;
  std::string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    do {
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> code >> vmSize >> idle;
      if (stream.eof()) {
        return "0";
      }
    } while (code.compare("VmSize:") != 0);
    stream.close();
    return (std::to_string(stoi(vmSize) / 1024));
  }
  return "";
}

// TODO: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  std::string label, uid;
  std::string line;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    do {
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> label >> uid;
    } while (label.compare("Uid:") != 0);
    stream.close();
    return uid;
  }
  return 0;
}

// TODO: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  std::string user, userid, encPass;
  std::string line;
  std::ifstream stream(kPasswordPath);

  string uid = LinuxParser::Uid(pid);

  if (stream.is_open()) {
    do {
      std::getline(stream, line);
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> encPass >> userid;
      if (stream.eof()) return "User not found";
    } while (userid.compare(uid) != 0);
    stream.close();
    return user;
  }
  return "0";
}

// TODO: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  char state;
  std::string comm;
  long ppid, pgrp, session, tty_nr, tpgid, flags, minflt, cminflt, majflt,
      cmajflt, utime, stime, cutime, cstime, priority, nice, num_threads,
      itrealvalue;
  std::string line;
  long startTime = 0;

  long clock = sysconf(_SC_CLK_TCK);

  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);

    linestream >> pid >> comm >> state >> ppid >> pgrp >> session >> tty_nr >>
        tpgid >> flags >> minflt >> cminflt >> majflt >> cmajflt >> utime >>
        stime >> cutime >> cstime >> priority >> nice >> num_threads >>
        itrealvalue >> startTime;
    stream.close();
  }

  return (startTime / clock);
}




