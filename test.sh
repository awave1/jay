#!/usr/bin/env bash

TEST_FILES=$(find "$PWD/test/data" -name "*.t*")
SCANNER=$PWD/scanner

echo -e "running tests...\n"
for f in $TEST_FILES; do
  echo "Testing: $f"
  SCANNER -f $f
done
echo -e "\ndone!"
