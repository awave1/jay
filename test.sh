#!/usr/bin/env bash

# Run the scanner against any *.t* files found in test/data directory

TEST_FILES=$(find "$PWD/test/data" -name "*.t*")
SCANNER=$PWD/scanner

echo -e "running tests...\n"
for f in $TEST_FILES; do
  echo "Testing: $f"
  $SCANNER $f
  echo -e "\n"
done
echo "done!"
