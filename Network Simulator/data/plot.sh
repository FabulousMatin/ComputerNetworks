#!/bin/bash

# Make the script exit if any command fails
set -e

# Run the first Python file
echo "Running first Python file..."
python3 cycles.py

# Run the second Python file
echo "Running second Python file..."
python3 forwarded.py

echo "Both scripts ran successfully."