#pragma once

#include <string>
#include <vector>

std::vector<std::string> split_import(std::string bstr);

enum LogLevel {
  log_critical,
  log_error,
  log_warning,
  log_info,
  log_debug
};

void dbp(int, const char * format, ...);

#define panic(x) _panic(x, __FILE_NAME__, __LINE__, __func__)
void _panic(std::string msg, std::string file_name, int line_no, std::string func_name);
