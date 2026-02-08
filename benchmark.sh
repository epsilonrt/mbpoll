#!/bin/bash
set -e

SERVER_BIN="./temp/libmodbus/tests/unit-test-server"
MBPOLL_BIN="./build/bin/mbpoll"
OUTPUT_FILE="benchmark_output.txt"

# Kill any existing server
killall unit-test-server 2>/dev/null || true

# Start server
$SERVER_BIN tcp 127.0.0.1 > /dev/null 2>&1 &
SERVER_PID=$!
sleep 1

echo "Running correctness check..."
$MBPOLL_BIN -m tcp -a 1 -r 400:1000 -t 4 -1 -o 1.0 -p 1502 127.0.0.1 > "$OUTPUT_FILE" 2>&1
kill $SERVER_PID 2>/dev/null || true
wait $SERVER_PID 2>/dev/null || true

# Check lines
line_count=$(grep -c "^\[" "$OUTPUT_FILE")
echo "Lines read: $line_count"
if [ "$line_count" -ne 601 ]; then
    echo "Error: Expected 601 lines, got $line_count"
    exit 1
fi

echo "Running performance benchmark (10 iterations)..."
start_time=$(date +%s%N)
for i in {1..10}; do
    $SERVER_BIN tcp 127.0.0.1 > /dev/null 2>&1 &
    SERVER_PID=$!
    # Wait for server to bind
    for retry in {1..50}; do
        if ss -lnt | grep -q ":1502 "; then
            break
        fi
        sleep 0.01
    done
    $MBPOLL_BIN -m tcp -a 1 -r 400:1000 -t 4 -1 -o 1.0 -p 1502 127.0.0.1 > /dev/null 2>&1
    kill $SERVER_PID 2>/dev/null || true
    wait $SERVER_PID 2>/dev/null || true
done
end_time=$(date +%s%N)

duration=$((end_time - start_time))
ms=$((duration / 1000000))
echo "Total Duration: $ms ms"
echo "Average per run: $((ms / 10)) ms"
