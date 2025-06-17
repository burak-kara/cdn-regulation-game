#!/usr/bin/env bash
set -o errexit
set -o nounset

# ------------------------------------------------------
# Kill all processes related to the game
# ------------------------------------------------------

DIR="$(dirname "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)")"
# Path to the executable to target
EXECUTABLE="$DIR/build/game"

# Inform the user
echo "Killing all processes matching: $EXECUTABLE"

# Send SIGTERM first
pkill -TERM -f "$EXECUTABLE" && echo "SIGTERM sent." || echo "No matching processes found."

# Wait a moment for graceful shutdown
sleep 3

# Force kill remaining processes, if any
if pgrep -f "$EXECUTABLE" > /dev/null; then
    echo "Processes still running. Sending SIGKILL..."
    pkill -KILL -f "$EXECUTABLE" && echo "SIGKILL sent." || echo "Failed to kill processes."
else
    echo "All processes terminated."
fi
