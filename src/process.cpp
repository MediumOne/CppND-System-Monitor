#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Return this process's ID
int Process::Pid() { return this->pid_; }

void Process::setPid(int pid) {
    this->pid_ = pid;
}

// Return this process's CPU utilization
float Process::CpuUtilization() { 
    //Get the CPU Utilization for the current process.
    cpuUtilization_ = LinuxParser::CpuUtilization(Pid());
    return cpuUtilization_;
}

// Return the command that generated this process
string Process::Command() { 
    //Get the command string for the current process.
    return LinuxParser::Command(Pid());
}

// Return this process's memory utilization
string Process::Ram() { 
    //Get the memory Utilization for the current process.
    return LinuxParser::Ram(Pid());
}

// Return the user (name) that generated this process
string Process::User() { 
    //Get the creator user name for the current process.
    return LinuxParser::User(Pid());
}

// Return the age of this process (in seconds)
long int Process::UpTime() { 
    //Get the uptime for the current process.
    return LinuxParser::UpTime(Pid());
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
    return a.cpuUtilization_ < cpuUtilization_; 
}