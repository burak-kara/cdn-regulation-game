#!/bin/bash
set -o errexit
set -o nounset

# ------------------------------------------------------
# Run the game with multiple configurations
# ------------------------------------------------------

DIR="$(dirname "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)")"
EXECUTABLE="$DIR/build/game"
CONFIG_DIR="$DIR/configs"

# Default config file
DEFAULT_CONFIG="$CONFIG_DIR/config.ini"

# Path to log file
DEFAULT_LOG_FILE="log"

# If no arguments, run with default config
if [ "$#" -eq 0 ]; then
    sudo nice -n -10 "$EXECUTABLE" --configFile="$DEFAULT_CONFIG" > $DEFAULT_LOG_FILE 2>&1 &
    cp $DEFAULT_LOG_FILE "log.default"
else
    # Iterate over all arguments and run the executable with corresponding config
    for arg in "$@"; do
        CONFIG_FILE="$CONFIG_DIR/config.${arg}.ini"
        sudo nice -n -10 "$EXECUTABLE" --configFile="$CONFIG_FILE" > $DEFAULT_LOG_FILE 2>&1
        cp $DEFAULT_LOG_FILE "log.${arg}"
    done
fi