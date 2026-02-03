#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

const std::string COOKIE_NAME = "cse135_state_cpp";

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

std::string url_decode(const std::string& s) {
  std::string out;
  for (size_t i = 0; i < s.size(); ++i) {
    if (s[i] == '+') {
      out += ' ';
    } else if (s[i] == '%' && i + 2 < s.size()) {
      std::string hex = s.substr(i + 1, 2);
      char c = static_cast<char>(std::strtol(hex.c_str(), nullptr, 16));
      out += c;
      i += 2;
    } else {
      out += s[i];
    }
  }
  return out;
}

std::map<std::string, std::string> parse_form(const std::string& data) {
  std::map<std::string, std::string> out;
  std::stringstream ss(data);
  std::string pair;
  while (std::getline(ss, pair, '&')) {
    auto pos = pair.find('=');
    std::string key = pos == std::string::npos ? pair : pair.substr(0, pos);
    std::string val = pos == std::string::npos ? "" : pair.substr(pos + 1);
    if (!key.empty()) {
      out[url_decode(key)] = url_decode(val);
    }
  }
  return out;
}

std::string read_body() {
  int len = 0;
  std::string lenStr = getenv_str("CONTENT_LENGTH");
  if (!lenStr.empty()) {
    len = std::atoi(lenStr.c_str());
  }
  if (len <= 0) return "";
  std::string body(len, '\0');
  std::cin.read(&body[0], len);
  return body;
}

std::map<std::string, std::string> read_cookie() {
  std::string cookie = getenv_str("HTTP_COOKIE");
  std::stringstream ss(cookie);
  std::string part;
  while (std::getline(ss, part, ';')) {
    while (!part.empty() && part.front() == ' ') part.erase(0, 1);
    auto pos = part.find('=');
    if (pos != std::string::npos) {
      std::string key = part.substr(0, pos);
      std::string val = part.substr(pos + 1);
      if (key == COOKIE_NAME) {
        return parse_form(val);
      }
    }
  }
  return {};
}

std::string url_encode_component(const std::string& s) {
  std::ostringstream out;
  for (unsigned char c : s) {
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') {
      out << c;
    } else if (c == ' ') {
      out << '+';
    } else {
      out << '%' << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << std::nouppercase << std::dec;
    }
  }
  return out.str();
}

std::string url_encode(const std::map<std::string, std::string>& data) {
  std::ostringstream out;
  bool first = true;
  for (const auto& kv : data) {
    if (!first) out << "&";
    first = false;
    out << url_encode_component(kv.first) << "=" << url_encode_component(kv.second);
  }
  return out.str();
}

int main() {
  std::string action = "view";
  auto qs = parse_form(getenv_str("QUERY_STRING"));
  if (qs.count("action")) action = qs["action"];

  auto current = read_cookie();
  auto newData = current;

  if (action == "set") {
    std::string method = getenv_str("REQUEST_METHOD");
    std::string contentType = getenv_str("CONTENT_TYPE");
    std::map<std::string, std::string> submitted;
    if ((method == "POST" || method == "PUT") && contentType.find("application/x-www-form-urlencoded") != std::string::npos) {
      submitted = parse_form(read_body());
    } else {
      submitted = parse_form(getenv_str("QUERY_STRING"));
    }
    if (submitted.count("name")) newData["name"] = submitted["name"];
    if (submitted.count("message")) newData["message"] = submitted["message"];
  } else if (action == "clear") {
    newData.clear();
  }

  std::cout << "Content-Type: text/html\r\n";
  if (action == "set") {
    std::cout << "Set-Cookie: " << COOKIE_NAME << "=" << url_encode(newData) << "; Path=/\r\n";
  } else if (action == "clear") {
    std::cout << "Set-Cookie: " << COOKIE_NAME << "=; Path=/; Expires=Thu, 01 Jan 1970 00:00:00 GMT\r\n";
  }
  std::cout << "\r\n";

  std::time_t t = std::time(nullptr);
  char buf[64];
  std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));

  std::cout << "<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><title>State - C++</title></head><body>";
  std::cout << "<h1>State (C++)</h1>";
  std::cout << "<p><strong>Time:</strong> " << html_escape(buf) << "</p>";
  std::cout << "<p><strong>IP:</strong> " << html_escape(client_ip()) << "</p>";

  std::cout << "<h2>Saved Data (Cookie)</h2>";
  if (newData.empty()) {
    std::cout << "<p><em>No data saved.</em></p>";
  } else {
    std::cout << "<ul>";
    for (const auto& kv : newData) {
      std::cout << "<li>" << html_escape(kv.first) << ": " << html_escape(kv.second) << "</li>";
    }
    std::cout << "</ul>";
  }

  std::cout << "<h2>Actions</h2><ul>";
  std::cout << "<li><a href='/hw2/state-cpp-form.html'>Set data</a></li>";
  std::cout << "<li><a href='/cgi-bin/state-cpp?action=view'>View here</a></li>";
  std::cout << "<li><a href='/cgi-bin/state-cpp?action=clear'>Clear data</a></li>";
  std::cout << "<li><a href='/hw2/state-cpp-view.html'>View page</a></li>";
  std::cout << "</ul></body></html>";
  return 0;
}
