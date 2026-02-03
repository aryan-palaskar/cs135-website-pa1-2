#!/usr/bin/env python3
import os
from datetime import datetime
from html import escape

def get_client_ip():
    xff = os.environ.get("HTTP_X_FORWARDED_FOR", "").split(",")[0].strip()
    return xff or os.environ.get("REMOTE_ADDR", "unknown")

team_members = ["Aryan Palaskar"]
lang = "Python"
now = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
ip = get_client_ip()

print("Content-Type: text/html\r\n")
print("<!DOCTYPE html>")
print("<html lang='en'><head><meta charset='utf-8'><title>Hello HTML - Python</title></head><body>")
print("<h1>Hello from Team Aryan</h1>")
print(f"<p><strong>Language:</strong> {escape(lang)}</p>")
print(f"<p><strong>Generated:</strong> {escape(now)}</p>")
print(f"<p><strong>IP:</strong> {escape(ip)}</p>")
print("<h2>Team Members</h2><ul>")
for m in team_members:
    print(f"<li>{escape(m)}</li>")
print("</ul>")
print("</body></html>")
