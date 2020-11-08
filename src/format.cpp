#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
    std::ostringstream formatTime;
    
    long hours = seconds / 3600;
    long remainingSec = seconds % 3600;
    long minutes = remainingSec / 60;
    remainingSec = remainingSec % 60;

    formatTime << std::setfill('0') << std::setw(2) << hours  << ":"  << std::setfill('0') << std::setw(2) 
                << minutes << ":" << std::setfill('0') << std::setw(2) << remainingSec;
    return formatTime.str();
}