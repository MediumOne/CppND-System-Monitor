#include "processor.h"
#include "linux_parser.h"

// Return the aggregate CPU utilization
float Processor::Utilization() {
    //Get the firsrt value and convert from string to float using 'stof'
    return LinuxParser::CpuUtilization();
}