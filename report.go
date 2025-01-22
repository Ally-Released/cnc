package main

import (
	"fmt"
	"log"
	"net"
	"strings"
	"time"
)

//cak3

const (
	serverAddress = "146.19.191.205:19"
	bufferSize    = 1024
)

func main() {
	report, err := net.Listen("tcp", serverAddress)
	if err != nil {
		log.Fatalf("Error starting server: %v", err)
	}

	defer report.Close()

	log.Printf("Report server listening on: %s", serverAddress)

	for {
		conn, err := report.Accept()
		if err != nil {
			log.Printf("Error accepting connection: %v", err)
			continue
		}
		go handleClient(conn)
	}
}

func handleClient(conn net.Conn) {
	defer conn.Close()

	buffer := make([]byte, bufferSize)

	for {
		n, err := conn.Read(buffer)
		if err != nil {
			log.Printf("Error reading from connection: %v", err)
			break
		}

		if len(string(buffer[:n])) > 5 {
			currentTime := time.Now().Format("2006-01-02 15:04:05")
			message := string(buffer[:n])

			var msgType string
			if strings.Contains(message, "Killed") {
				msgType = "Killer"
			} else {
				msgType = "unknown"
			}

			output := fmt.Sprintf("\x1b[96m%s\x1b[0m~/ [\x1b[96m%s\x1b[0m] %s", currentTime, msgType, message[:min(len(message), 100)])
			log.Println(output)
		}
	}
}

func min(a, b int) int {
	if a < b {
		return a
	}
	return b
}
