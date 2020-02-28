#!/usr/bin/env bash

# Run the scanner against any *.t* files found in test/data directory

TEST_FILES=$(find "$PWD/test/$1" -name "*.*")
COMPILER=$PWD/jay

echo -e "running tests...\n"
for f in $TEST_FILES; do
  # cat $f
  echo "$f: AST"
  $COMPILER $f
  echo -e "------------------------------------\n"
done
echo "done!"
