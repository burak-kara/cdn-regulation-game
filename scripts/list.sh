#!/usr/bin/env bash
set -o errexit
set -o nounset

# ------------------------------------------------------
# List all running processes related to the game
# ------------------------------------------------------

ps -eLf | grep '[g]ame' || echo "No running 'game' processes found."
