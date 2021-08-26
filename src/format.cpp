#include <string>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 

 string output;
 long hours{0}, mins{0}, secs{0};
 string hours_string, mins_string, secs_string;
 hours = seconds / 3600;
 hours_string = std::to_string(hours).length() > 1 ? std::to_string(hours) : "0" + std::to_string(hours);
 mins = (seconds%3600)/60;
 mins_string = std::to_string(mins).length() > 1 ? std::to_string(mins) : "0" + std::to_string(mins);
 secs = (seconds%3600)%60;
 secs_string = std::to_string(secs).length() > 1 ? std::to_string(secs) : "0" + std::to_string(secs);

 output = hours_string + ":" + mins_string + ":" + secs_string;
 return output;
}