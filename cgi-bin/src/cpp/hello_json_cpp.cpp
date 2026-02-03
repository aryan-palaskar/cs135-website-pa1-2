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

std::string json_escape(const std::string& s) {
  std::ostringstream out;
  for (char c : s) {
    switch (c) {
      case '"': out << "\\\""; break;
      case '\\': out << "\\\\"; break;
      case '\b': out << "\\b"; break;
      case '\f': out << "\\f"; break;
      case '\n': out << "\\n"; break;
      case '\r': out << "\\r"; break;
      case '\t': out << "\\t"; break;
      default: out << c; break;
    }
  }
  return out.str();
}

int main() {
  std::time_t t = std::time(nullptr);
  char buf[64];
  std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));

  std::cout << "Content-Type: application/json\r\n\r\n";
  std::cout << "{\n";
  std::cout << "  \"greeting\": \"Hello from Team Aryan\",\n";
  std::cout << "  \"team_members\": [\"" << json_escape("Aryan Palaskar") << "\"],\n";
  std::cout << "  \"language\": \"C++\",\n";
  std::cout << "  \"generated\": \"" << json_escape(buf) << "\",\n";
  std::cout << "  \"ip\": \"" << json_escape(client_ip()) << "\"\n";
  std::cout << "}\n";
  return 0;
}
