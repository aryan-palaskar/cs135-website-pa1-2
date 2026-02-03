#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

extern char **environ;

std::string html_escape(const std::string& s) {
  std::string out;
  for (char c : s) {
    switch (c) {
      case '&': out += "&amp;"; break;
      case '<': out += "&lt;"; break;
      case '>': out += "&gt;"; break;
      case '"': out += "&quot;"; break;
      case '\'': out += "&#39;"; break;
      default: out += c; break;
    }
  }
  return out;
}

int main() {
  std::vector<std::string> envs;
  for (char **env = environ; *env != nullptr; ++env) {
    envs.emplace_back(*env);
  }
  std::sort(envs.begin(), envs.end());

  std::cout << "Content-Type: text/html\r\n\r\n";
  std::cout << "<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><title>Environment - C++</title></head><body>";
  std::cout << "<h1>Request Environment (C++)</h1>";
  std::cout << "<table border='1' cellpadding='6' cellspacing='0'>";
  std::cout << "<thead><tr><th>Variable</th><th>Value</th></tr></thead><tbody>";
  for (const auto& kv : envs) {
    auto pos = kv.find('=');
    std::string key = pos == std::string::npos ? kv : kv.substr(0, pos);
    std::string val = pos == std::string::npos ? "" : kv.substr(pos + 1);
    std::cout << "<tr><td>" << html_escape(key) << "</td><td>" << html_escape(val) << "</td></tr>";
  }
  std::cout << "</tbody></table></body></html>";
  return 0;
}
