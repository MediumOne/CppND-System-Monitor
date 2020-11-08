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
int Process::Pid() const { return this->pid_; }

void Process::setPid(int pid) {
    this->pid_ = pid;
}

// Return this process's CPU utilization
float Process::CpuUtilization() const { 
    float totaltime = LinuxParser::ActiveJiffies(Pid());  // In jiffies
    float secondsactive = this->Process::UpTime();  // In seconds
    long cpu_usage_ = (totaltime / sysconf(_SC_CLK_TCK)) / secondsactive;
    return cpu_usage_;
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
long int Process::UpTime() const { 
    //Get the uptime for the current process.
    return LinuxParser::UpTime(Pid());
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
    return CpuUtilization() < a.CpuUtilization();
}