#!/usr/bin/env bash

TEST_FILES=$(find "$PWD/test/data" -name "*.t*")
SCANNER=$PWD/scanner

RED=$(tput setaf 1)
GREEN=$(tput setaf 2)
RESET=$(tput sgr0)

echo -e "running tests...\n"
for f in $TEST_FILES; do
  echo "Testing: $f"
  SCANNER -f $f
  if [ $? -eq 0 ]; then
    echo "${GREEN}PASS${RESET}"
  else
    echo "${RED}FAIL${RESET}"
  fi
  echo "------------------------------"
done
echo -e "\ndone!"
