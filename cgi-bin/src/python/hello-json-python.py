#!/usr/bin/env python3
import os
import json
from datetime import datetime

def get_client_ip():
    xff = os.environ.get("HTTP_X_FORWARDED_FOR", "").split(",")[0].strip()
    return xff or os.environ.get("REMOTE_ADDR", "unknown")

payload = {
    "greeting": "Hello from Team Aryan",
    "team_members": ["Aryan Palaskar"],
    "language": "Python",
    "generated": datetime.now().strftime("%Y-%m-%d %H:%M:%S"),
    "ip": get_client_ip(),
}

print("Content-Type: application/json\r\n")
print(json.dumps(payload, indent=2))
