package main

import (
	"encoding/json"
	"fmt"
	"html"
	"io"
	"net/url"
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

func readBody() string {
	length := 0
	fmt.Sscanf(os.Getenv("CONTENT_LENGTH"), "%d", &length)
	if length <= 0 {
		return ""
	}
	buf := make([]byte, length)
	_, _ = io.ReadFull(os.Stdin, buf)
	return string(buf)
}

func parseForm(data string) map[string]any {
	out := map[string]any{}
	vals, _ := url.ParseQuery(data)
	for k, v := range vals {
		if len(v) == 1 {
			out[k] = v[0]
		} else {
			out[k] = v
		}
	}
	return out
}

func main() {
	method := os.Getenv("REQUEST_METHOD")
	query := parseForm(os.Getenv("QUERY_STRING"))
	contentType := os.Getenv("CONTENT_TYPE")
	body := readBody()

	var parsedBody any
	if strings.Contains(contentType, "application/json") {
		var obj any
		if body != "" && json.Unmarshal([]byte(body), &obj) == nil {
			parsedBody = obj
		} else if body == "" {
			parsedBody = map[string]any{}
		} else {
			parsedBody = map[string]any{"_error": "invalid json", "raw": body}
		}
	} else if strings.Contains(contentType, "application/x-www-form-urlencoded") {
		parsedBody = parseForm(body)
	} else {
		parsedBody = map[string]any{"raw": body}
	}

	fmt.Print("Content-Type: text/html\r\n\r\n")
	fmt.Print("<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><title>Echo - Go</title></head><body>")
	fmt.Print("<h1>Echo (Go)</h1>")
	fmt.Print("<ul>")
	fmt.Printf("<li><strong>Host:</strong> %s</li>", html.EscapeString(os.Getenv("HTTP_HOST")))
	fmt.Printf("<li><strong>Time:</strong> %s</li>", html.EscapeString(time.Now().Format("2006-01-02 15:04:05")))
	fmt.Printf("<li><strong>Method:</strong> %s</li>", html.EscapeString(method))
	fmt.Printf("<li><strong>User-Agent:</strong> %s</li>", html.EscapeString(os.Getenv("HTTP_USER_AGENT")))
	fmt.Printf("<li><strong>IP:</strong> %s</li>", html.EscapeString(clientIP()))
	fmt.Print("</ul>")

	fmt.Print("<h2>Query Parameters</h2><pre>")
	qb, _ := json.MarshalIndent(query, "", "  ")
	fmt.Print(html.EscapeString(string(qb)))
	fmt.Print("</pre>")

	fmt.Print("<h2>Body</h2><pre>")
	bb, _ := json.MarshalIndent(parsedBody, "", "  ")
	fmt.Print(html.EscapeString(string(bb)))
	fmt.Print("</pre>")

	if body != "" {
		fmt.Print("<h3>Raw Body</h3><pre>")
		fmt.Print(html.EscapeString(body))
		fmt.Print("</pre>")
	}

	fmt.Print("</body></html>")
}
