#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

std::string getenv_str(const char* key) {
  const char* val = std::getenv(key);
  return val ? val : "";
}

std::string client_ip() {
  std::string xff = getenv_str("HTTP_X_FORWARDED_FOR");
  if (!xff.empty()) {
    auto pos = xff.find(',');
    if (pos != std::string::npos) xff = xff.substr(0, pos);
    while (!xff.empty() && xff.front() == ' ') xff.erase(0, 1);
    while (!xff.empty() && xff.back() == ' ') xff.pop_back();
    return xff;
  }
  std::string ip = getenv_str("REMOTE_ADDR");
  return ip.empty() ? "unknown" : ip;
}

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
  std::vector<std::string> team = {"Aryan Palaskar"};
  std::time_t t = std::time(nullptr);
  char buf[64];
  std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));

  std::cout << "Content-Type: text/html\r\n\r\n";
  std::cout << "<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><title>Hello HTML - C++</title></head><body>";
  std::cout << "<h1>Hello from Team Aryan</h1>";
  std::cout << "<p><strong>Language:</strong> " << html_escape("C++") << "</p>";
  std::cout << "<p><strong>Generated:</strong> " << html_escape(buf) << "</p>";
  std::cout << "<p><strong>IP:</strong> " << html_escape(client_ip()) << "</p>";
  std::cout << "<h2>Team Members</h2><ul>";
  for (const auto& m : team) {
    std::cout << "<li>" << html_escape(m) << "</li>";
  }
  std::cout << "</ul></body></html>";
  return 0;
}
