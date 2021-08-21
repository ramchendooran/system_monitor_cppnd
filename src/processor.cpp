#include "processor.h"
#include <iostream>
// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    
    string cpu, idle, iowait, user, nice, system, irq, softirq, steal;
    string line;
    std::ifstream stream(LinuxParser::kProcDirectory + LinuxParser::kStatFilename);
    if (stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    }
    
    float PrevIdle = idle_ + iowait_;
    float Idle = stof(idle) + stof(iowait);

    float PrevNonIdle = user_ + nice_ + system_ + irq_ + softirq_ + steal_;
    float NonIdle = stof(user) + stof(nice) + stof(system) + stof(irq) + stof(softirq) + stof(steal);

    float PrevTotal = PrevIdle + PrevNonIdle;
    float Total = Idle + NonIdle;

    float totald = Total - PrevTotal;
    float idled = Idle - PrevIdle;

    float CPU_Percentage = (totald - idled)/totald;

    // Persist previous variables
    idle_ = stof(idle);
    iowait_ = stof(iowait);
    user_ = stof(user);
    nice_ = stof(nice);
    system_ = stof(system);
    irq_ = stof(irq);
    softirq_ = stof(softirq);
    steal_ = stof(steal);

    return CPU_Percentage;
    std::cout << CPU_Percentage;
}