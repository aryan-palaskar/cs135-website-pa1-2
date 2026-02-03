package main

import (
	"fmt"
	"html"
	"os"
	"sort"
	"strings"
)

func main() {
	fmt.Print("Content-Type: text/html\r\n\r\n")
	fmt.Print("<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><title>Environment - Go</title></head><body>")
	fmt.Print("<h1>Request Environment (Go)</h1>")
	fmt.Print("<table border='1' cellpadding='6' cellspacing='0'><thead><tr><th>Variable</th><th>Value</th></tr></thead><tbody>")

	env := os.Environ()
	sort.Strings(env)
	for _, kv := range env {
		parts := strings.SplitN(kv, "=", 2)
		key := parts[0]
		val := ""
		if len(parts) > 1 {
			val = parts[1]
		}
		fmt.Printf("<tr><td>%s</td><td>%s</td></tr>", html.EscapeString(key), html.EscapeString(val))
	}

	fmt.Print("</tbody></table></body></html>")
}
