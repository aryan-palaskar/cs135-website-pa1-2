package main

import (
	"fmt"
	"html"
	"io"
	"net/url"
	"os"
	"strings"
	"time"
)

const cookieName = "cse135_state_go"

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

func parseForm(data string) map[string]string {
	out := map[string]string{}
	vals, _ := url.ParseQuery(data)
	for k, v := range vals {
		if len(v) > 0 {
			out[k] = v[0]
		}
	}
	return out
}

func readCookie(name string) map[string]string {
	cookies := os.Getenv("HTTP_COOKIE")
	if cookies == "" {
		return map[string]string{}
	}
	parts := strings.Split(cookies, ";")
	for _, p := range parts {
		p = strings.TrimSpace(p)
		kv := strings.SplitN(p, "=", 2)
		if len(kv) == 2 && kv[0] == name {
			return parseForm(kv[1])
		}
	}
	return map[string]string{}
}

func main() {
	action := "view"
	qs := url.Values{}
	if os.Getenv("QUERY_STRING") != "" {
		qs, _ = url.ParseQuery(os.Getenv("QUERY_STRING"))
	}
	if v := qs.Get("action"); v != "" {
		action = v
	}

	current := readCookie(cookieName)
	newData := map[string]string{}
	for k, v := range current {
		newData[k] = v
	}

	if action == "set" {
		method := os.Getenv("REQUEST_METHOD")
		contentType := os.Getenv("CONTENT_TYPE")
		var submitted map[string]string
		if (method == "POST" || method == "PUT") && strings.Contains(contentType, "application/x-www-form-urlencoded") {
			submitted = parseForm(readBody())
		} else {
			submitted = parseForm(os.Getenv("QUERY_STRING"))
		}
		for _, key := range []string{"name", "message"} {
			if v := submitted[key]; v != "" {
				newData[key] = v
			}
		}
	} else if action == "clear" {
		newData = map[string]string{}
	}

	fmt.Print("Content-Type: text/html\r\n")
	if action == "set" || action == "clear" {
		cookieVal := url.Values{}
		for k, v := range newData {
			cookieVal.Set(k, v)
		}
		if action == "clear" {
			fmt.Printf("Set-Cookie: %s=; Path=/; Expires=Thu, 01 Jan 1970 00:00:00 GMT\r\n", cookieName)
		} else {
			fmt.Printf("Set-Cookie: %s=%s; Path=/\r\n", cookieName, cookieVal.Encode())
		}
	}
	fmt.Print("\r\n")

	fmt.Print("<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><title>State - Go</title></head><body>")
	fmt.Print("<h1>State (Go)</h1>")
	fmt.Printf("<p><strong>Time:</strong> %s</p>", html.EscapeString(time.Now().Format("2006-01-02 15:04:05")))
	fmt.Printf("<p><strong>IP:</strong> %s</p>", html.EscapeString(clientIP()))
	fmt.Print("<h2>Saved Data (Cookie)</h2>")
	if len(newData) == 0 {
		fmt.Print("<p><em>No data saved.</em></p>")
	} else {
		fmt.Print("<ul>")
		for k, v := range newData {
			fmt.Printf("<li>%s: %s</li>", html.EscapeString(k), html.EscapeString(v))
		}
		fmt.Print("</ul>")
	}
	fmt.Print("<h2>Actions</h2><ul>")
	fmt.Print("<li><a href='/hw2/state-go-form.html'>Set data</a></li>")
	fmt.Print("<li><a href='/cgi-bin/state-go?action=view'>View here</a></li>")
	fmt.Print("<li><a href='/cgi-bin/state-go?action=clear'>Clear data</a></li>")
	fmt.Print("<li><a href='/hw2/state-go-view.html'>View page</a></li>")
	fmt.Print("</ul></body></html>")
}
