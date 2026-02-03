#!/usr/bin/env python3
import os
from urllib.parse import parse_qs, urlencode
from http.cookies import SimpleCookie
from html import escape
from datetime import datetime

COOKIE_NAME = "cse135_state_python"


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


def parse_form(data):
    return {k: v if len(v) > 1 else v[0] for k, v in parse_qs(data, keep_blank_values=True).items()}


action = parse_qs(os.environ.get("QUERY_STRING", "")).get("action", ["view"])[0]
method = os.environ.get("REQUEST_METHOD", "GET")
content_type = os.environ.get("CONTENT_TYPE", "")

cookie = SimpleCookie(os.environ.get("HTTP_COOKIE", ""))
current = {}
if COOKIE_NAME in cookie:
    current = parse_form(cookie[COOKIE_NAME].value)

new_data = current.copy()

if action == "set":
    body = read_body() if method in ("POST", "PUT") else ""
    if "application/x-www-form-urlencoded" in content_type:
        submitted = parse_form(body)
    else:
        submitted = parse_form(os.environ.get("QUERY_STRING", ""))
    for key in ("name", "message"):
        if key in submitted and submitted[key] != "":
            new_data[key] = submitted[key]

elif action == "clear":
    new_data = {}

headers = ["Content-Type: text/html"]
if action in ("set", "clear"):
    c = SimpleCookie()
    c[COOKIE_NAME] = urlencode(new_data)
    c[COOKIE_NAME]["path"] = "/"
    if action == "clear":
        c[COOKIE_NAME]["expires"] = "Thu, 01 Jan 1970 00:00:00 GMT"
    morsel = c[COOKIE_NAME]
    headers.append(f"Set-Cookie: {morsel.OutputString()}")

print("\r\n".join(headers) + "\r\n\r\n")
print("<!DOCTYPE html>")
print("<html lang='en'><head><meta charset='utf-8'><title>State - Python</title></head><body>")
print("<h1>State (Python)</h1>")
print(f"<p><strong>Time:</strong> {escape(datetime.now().strftime('%Y-%m-%d %H:%M:%S'))}</p>")
print(f"<p><strong>IP:</strong> {escape(get_client_ip())}</p>")

print("<h2>Saved Data (Cookie)</h2>")
if new_data:
    print("<ul>")
    for k, v in new_data.items():
        print(f"<li>{escape(k)}: {escape(str(v))}</li>")
    print("</ul>")
else:
    print("<p><em>No data saved.</em></p>")

print("<h2>Actions</h2>")
print("<ul>")
print("<li><a href='/hw2/state-python-form.html'>Set data</a></li>")
print("<li><a href='/cgi-bin/state-python.py?action=view'>View here</a></li>")
print("<li><a href='/cgi-bin/state-python.py?action=clear'>Clear data</a></li>")
print("<li><a href='/hw2/state-python-view.html'>View page</a></li>")
print("</ul>")

print("</body></html>")
