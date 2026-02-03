#!/usr/bin/env python3
import os
from html import escape

print("Content-Type: text/html\r\n")
print("<!DOCTYPE html>")
print("<html lang='en'><head><meta charset='utf-8'><title>Environment - Python</title></head><body>")
print("<h1>Request Environment (Python)</h1>")
print("<table border='1' cellpadding='6' cellspacing='0'>")
print("<thead><tr><th>Variable</th><th>Value</th></tr></thead><tbody>")
for key in sorted(os.environ.keys()):
    val = os.environ.get(key, "")
    print(f"<tr><td>{escape(key)}</td><td>{escape(val)}</td></tr>")
print("</tbody></table>")
print("</body></html>")
