#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>

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
    out[url_decode(key)] = url_decode(val);
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

int main() {
  std::string method = getenv_str("REQUEST_METHOD");
  std::string queryStr = getenv_str("QUERY_STRING");
  std::string contentType = getenv_str("CONTENT_TYPE");
  std::string body = read_body();

  auto query = parse_form(queryStr);
  std::map<std::string, std::string> bodyMap;
  if (contentType.find("application/x-www-form-urlencoded") != std::string::npos) {
    bodyMap = parse_form(body);
  }

  std::time_t t = std::time(nullptr);
  char buf[64];
  std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));

  std::cout << "Content-Type: text/html\r\n\r\n";
  std::cout << "<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><title>Echo - C++</title></head><body>";
  std::cout << "<h1>Echo (C++)</h1>";
  std::cout << "<ul>";
  std::cout << "<li><strong>Host:</strong> " << html_escape(getenv_str("HTTP_HOST")) << "</li>";
  std::cout << "<li><strong>Time:</strong> " << html_escape(buf) << "</li>";
  std::cout << "<li><strong>Method:</strong> " << html_escape(method) << "</li>";
  std::cout << "<li><strong>User-Agent:</strong> " << html_escape(getenv_str("HTTP_USER_AGENT")) << "</li>";
  std::cout << "<li><strong>IP:</strong> " << html_escape(client_ip()) << "</li>";
  std::cout << "</ul>";

  std::cout << "<h2>Query Parameters</h2><pre>";
  for (const auto& kv : query) {
    std::cout << html_escape(kv.first) << " = " << html_escape(kv.second) << "\n";
  }
  std::cout << "</pre>";

  std::cout << "<h2>Body</h2><pre>";
  if (!bodyMap.empty()) {
    for (const auto& kv : bodyMap) {
      std::cout << html_escape(kv.first) << " = " << html_escape(kv.second) << "\n";
    }
  } else if (!body.empty()) {
    std::cout << html_escape(body) << "\n";
  }
  std::cout << "</pre>";

  if (!body.empty()) {
    std::cout << "<h3>Raw Body</h3><pre>" << html_escape(body) << "</pre>";
  }

  std::cout << "</body></html>";
  return 0;
}
