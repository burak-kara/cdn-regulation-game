#!/usr/bin/env bash
set -o errexit
set -o nounset
set -euo pipefail

# ------------------------------------------------------
# Run the game with a specified config
# ------------------------------------------------------

DIR="$(dirname "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)")"
EXECUTABLE="$DIR/build/game"
CONFIG_DIR="$DIR/configs"

echo "$CONFIG_DIR"

# pick config
if [[ $# -eq 0 ]]; then
  echo "Warning: no config specified, exiting!" >&2
  echo "Usage: $0 <config_name>" >&2
  echo "Available configs:" >&2
  for cfg in "$CONFIG_DIR"/config.*.ini; do
    name=$(basename "$cfg" | sed -e 's/^config\.//' -e 's/\.ini$//')
    echo " - $name" >&2
  done
  exit 1
else
  CONFIG_FILE="$CONFIG_DIR/config.$1.ini"
  DEFAULT_LOG_FILE="logs/log"
  LOG_FILE="$DIR/logs/log.$1"
fi

# sanity checks
if [[ ! -x "$EXECUTABLE" ]]; then
  echo "Error: executable not found or not executable: $EXECUTABLE" >&2
  exit 1
fi
if [[ ! -f "$CONFIG_FILE" ]]; then
  echo "Error: config not found: $CONFIG_FILE" >&2
  exit 1
fi
mkdir -p "$(dirname "$LOG_FILE")"

# run and log
"$EXECUTABLE" --configFile="$CONFIG_FILE" > "$DEFAULT_LOG_FILE" 2>&1
cp "$DEFAULT_LOG_FILE" "$LOG_FILE";
