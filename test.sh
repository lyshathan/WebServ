#!/bin/bash

# Configurable parameters
URL="http://127.0.0.1:8080/cgi/index.sh"
CONCURRENCY=10
REPS=100

echo "Checking for siege..."
if ! command -v siege >/dev/null 2>&1; then
    echo "siege not found. Installing with Homebrew..."
    brew install siege || { echo "Failed to install siege"; exit 1; }
fi

echo "Setting ulimit for open files and user processes..."
ulimit -n 4096
ulimit -u 2048

echo "Starting stress test:"
echo "  URL: $URL"
echo "  Concurrency: $CONCURRENCY"
echo "  Repetitions: $REPS"
echo "--------------------------------------"

siege -c $CONCURRENCY -r $REPS $URL

echo "--------------------------------------"
echo "Stress test complete."