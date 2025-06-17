#!/usr/bin/env bash
set -o errexit
set -o nounset

# ------------------------------------------------------
# Clear all data and logs related to the game
# ------------------------------------------------------

DIR="$(dirname "$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)")"

echo "Cleaning data* log* nohup.out and *log files in $DIR"

# Delete all data* folders
find . -type d -name 'data*' -exec rm -rf {} +

# Delete all log* folders
find . -type d -name 'log*' -exec rm -rf {} +

# Delete nohup.out file if exists
if [ -f nohup.out ]; then
  rm -f nohup.out
  echo "Deleted nohup.out"
fi

# Delete all *log files
find . -type f -name '*log' -exec rm -f {} +

echo "Clear complete."
