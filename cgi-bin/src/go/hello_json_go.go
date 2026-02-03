package main

import (
	"encoding/json"
	"fmt"
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
	payload := map[string]any{
		"greeting":     "Hello from Team Aryan",
		"team_members": []string{"Aryan Palaskar"},
		"language":     "Go",
		"generated":    time.Now().Format("2006-01-02 15:04:05"),
		"ip":           clientIP(),
	}
	fmt.Print("Content-Type: application/json\r\n\r\n")
	enc := json.NewEncoder(os.Stdout)
	enc.SetIndent("", "  ")
	_ = enc.Encode(payload)
}
