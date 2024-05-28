#!/bin/bash

cat << "EOF"
┌─┐┌─┐┌─┐┌┬┐┌─┐
├─┘├─┤└─┐ │ │ │
┴  ┴ ┴└─┘ ┴ └─┘
EOF

# Function to display usage information
usage() {
  echo "Usage: $0 [-o] [directory] filename"
  exit 1
}

# Initialize variables
OVERWRITE=0
ARGS=()

# Parse options and arguments
for arg in "$@"; do
  case $arg in
    -o)
      OVERWRITE=1
      ;;
    *)
      ARGS+=("$arg")
      ;;
  esac
done

# Check the number of arguments provided
if [ ${#ARGS[@]} -eq 0 ]; then
  usage
elif [ ${#ARGS[@]} -eq 1 ]; then
  # Only one argument provided: filename
  FILENAME=${ARGS[0]}
  DIRECTORY=""
else
  # Two arguments provided: directory and filename
  DIRECTORY=${ARGS[0]}
  FILENAME=${ARGS[1]}
fi

# Create the directory if it does not exist and a directory is specified
if [ ! -z "$DIRECTORY" ]; then
  if [ ! -e "$DIRECTORY" ]; then
    mkdir -p "$DIRECTORY"
    if [ $? -ne 0 ]; then
      echo "Error: Failed to create directory '$DIRECTORY'."
      exit 1
    fi
  elif [ ! -d "$DIRECTORY" ]; then
    echo "Error: '$DIRECTORY' exists and is not a directory."
    exit 1
  fi

  # Set the full file path
  FILEPATH="$DIRECTORY/$FILENAME"
else
  # No directory specified, just use the filename
  FILEPATH="$FILENAME"
fi

# Check if file already exists and handle overwrite flag
if [ -e "$FILEPATH" ] && [ $OVERWRITE -ne 1 ]; then
  echo "Error: The file '$FILEPATH' already exists. Use -o to overwrite."
  exit 1
fi

# Get clipboard content and write to file
./clipboard > "$FILEPATH"

if [ $? -eq 0 ]; then
  echo "File '$FILEPATH' created successfully with clipboard content."
else
  echo "Error: Failed to retrieve clipboard content."
  exit 1
fi

