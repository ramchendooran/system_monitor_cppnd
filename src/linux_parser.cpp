#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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
  string key;
  string value;
  float memory_total;
  float memory_free;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    bool strfound = false;
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          memory_total = std::stof(value);
          break;
        }
        else if (key == "MemFree:") {
          memory_free = std::stof(value);
          strfound = true;
          break;
        }
      }
      if (strfound) {break;}
    }
  }
  return (((memory_total - memory_free)/memory_total)); 
}

//  Read and return the system uptime
long LinuxParser::UpTime() { 
  string time_up, time_idle;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> time_up >> time_idle;
  }
  return std::stol(time_up);
 }

// Function to split string by space delimiter
void splitString(const string &str, vector<string> &output)
{
    string::size_type start = 0; 
    string::size_type last = str.find_first_of(" "); 

    while (last != string::npos) {

        if (last > start) {
            output.push_back(str.substr(start, last - start)); 
        }

        start = ++last; 
        last = str.find_first_of(" ", last); 
    }
    
    output.push_back(str.substr(start));
}

//  Read and return CPU utilization
float LinuxParser::CpuUtilization(int pid) { 
  
  string line;
  vector<string> words;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
  }
  splitString(line, words);

  float uptime = LinuxParser::UpTime();
  float utime = stof(words[14]);
  float stime = stof(words[15]);
  float cutime = stof(words[16]);
  float cstime = stof(words[17]);
  float starttime = stof(words[22]); 

  float total_time = utime + stime + cutime + cstime;
  float seconds = uptime - (starttime/sysconf(_SC_CLK_TCK));
  float cpuUtil = ((total_time /sysconf(_SC_CLK_TCK)) / seconds);

  return cpuUtil;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  string key;
  string value = "0";
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return std::stoi(value);
        }
      }
    }
  }
  return std::stoi(value); 
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line;
  string key;
  string value = "0";
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return std::stoi(value);
        }
      }
    }
  }
  return std::stoi(value); 
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
  }
  return line; 
}

//  Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string line;
  string key;
  string value = "0";
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          return std::to_string(std::stol(value)/1000);
        }
      }
    }
  }
  return value; 
}

//  Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string line;
  string key;
  string value = "0";
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }
  return value; 
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string uid = Uid(pid);
  string line;
  string key, x;
  string value = "0";
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> x >> value) {
        if (value == uid) {
          return key;
        }
      }
    }
  }
  return "Invalid Uid"; 
}

//  Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  string line;
  vector<string> words;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
  }
  splitString(line, words);

  return UpTime() - stol(words[21])/sysconf(_SC_CLK_TCK);
}
