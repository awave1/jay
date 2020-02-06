#!/usr/bin/env bash

TEST_FILES=$(find "$PWD/test/data" -name "*.t*")
SCANNER=$PWD/scanner

for f in $TEST_FILES; do
  echo $f
  SCANNER -f $f
done
