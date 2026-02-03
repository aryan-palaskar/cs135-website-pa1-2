#!/usr/bin/env python3
import os
import json
from urllib.parse import parse_qs
from html import escape
from datetime import datetime


def get_client_ip():
    xff = os.environ.get("HTTP_X_FORWARDED_FOR", "").split(",")[0].strip()
    return xff or os.environ.get("REMOTE_ADDR", "unknown")


def read_body():
    try:
        length = int(os.environ.get("CONTENT_LENGTH", "0"))
    except ValueError:
        length = 0
    if length > 0:
        return os.sys.stdin.read(length)
    return ""


def parse_body(body, content_type):
    if "application/json" in content_type:
        try:
            return json.loads(body) if body else {}
        except json.JSONDecodeError:
            return {"_error": "invalid json", "raw": body}
    if "application/x-www-form-urlencoded" in content_type:
        return {k: v if len(v) > 1 else v[0] for k, v in parse_qs(body, keep_blank_values=True).items()}
    return {"raw": body}


method = os.environ.get("REQUEST_METHOD", "GET")
query = parse_qs(os.environ.get("QUERY_STRING", ""), keep_blank_values=True)
content_type = os.environ.get("CONTENT_TYPE", "")
body = read_body()
parsed_body = parse_body(body, content_type)

host = os.environ.get("HTTP_HOST", "")
ua = os.environ.get("HTTP_USER_AGENT", "")
now = datetime.now().strftime("%Y-%m-%d %H:%M:%S")

print("Content-Type: text/html\r\n")
print("<!DOCTYPE html>")
print("<html lang='en'><head><meta charset='utf-8'><title>Echo - Python</title></head><body>")
print("<h1>Echo (Python)</h1>")
print("<ul>")
print(f"<li><strong>Host:</strong> {escape(host)}</li>")
print(f"<li><strong>Time:</strong> {escape(now)}</li>")
print(f"<li><strong>Method:</strong> {escape(method)}</li>")
print(f"<li><strong>User-Agent:</strong> {escape(ua)}</li>")
print(f"<li><strong>IP:</strong> {escape(get_client_ip())}</li>")
print("</ul>")

print("<h2>Query Parameters</h2>")
print("<pre>")
print(escape(json.dumps({k: v if len(v) > 1 else v[0] for k, v in query.items()}, indent=2)))
print("</pre>")

print("<h2>Body</h2>")
print("<pre>")
print(escape(json.dumps(parsed_body, indent=2) if isinstance(parsed_body, (dict, list)) else str(parsed_body)))
print("</pre>")

if body:
    print("<h3>Raw Body</h3>")
    print("<pre>")
    print(escape(body))
    print("</pre>")

print("</body></html>")
