#!/usr/bin/env bash

SCANNER=''
TEST_FILES=$(find "$(dirname $0)/data" -name "*.t*")

for f in $TEST_FILES; do
  echo $f
done
