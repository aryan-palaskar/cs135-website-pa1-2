package main

import (
	"fmt"
	"html"
	"os"
	"strings"
	"time"
)

func clientIP() string {
	xff := os.Getenv("HTTP_X_FORWARDED_FOR")
	if xff != "" {
		parts := strings.Split(xff, ",")
		if len(parts) > 0 {
			return strings.TrimSpace(parts[0])
		}
	}
	ip := os.Getenv("REMOTE_ADDR")
	if ip == "" {
		return "unknown"
	}
	return ip
}

func main() {
	team := []string{"Aryan Palaskar"}
	fmt.Print("Content-Type: text/html\r\n\r\n")
	fmt.Print("<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><title>Hello HTML - Go</title></head><body>")
	fmt.Print("<h1>Hello from Team Aryan</h1>")
	fmt.Printf("<p><strong>Language:</strong> %s</p>", html.EscapeString("Go"))
	fmt.Printf("<p><strong>Generated:</strong> %s</p>", html.EscapeString(time.Now().Format("2006-01-02 15:04:05")))
	fmt.Printf("<p><strong>IP:</strong> %s</p>", html.EscapeString(clientIP()))
	fmt.Print("<h2>Team Members</h2><ul>")
	for _, m := range team {
		fmt.Printf("<li>%s</li>", html.EscapeString(m))
	}
	fmt.Print("</ul></body></html>")
}
