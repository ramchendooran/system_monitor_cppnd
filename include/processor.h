#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <sstream>
#include <string>
#include <fstream>
#include "linux_parser.h"

using std::string;
using std::stoi;
class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  float idle_{0.0}, iowait_{0.0}, user_{0.0}, nice_{0.0}, system_{0.0}, irq_{0.0}, softirq_{0.0}, steal_{0.0};
};

#endif