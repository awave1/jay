#!/usr/bin/env bash

# Run the scanner against any *.t* files found in test/data directory

TEST_FILES=$(find "$PWD/test/$1" -name "*.*")
COMPILER=$PWD/jay

make build

echo -e "running tests...\n"
for f in $TEST_FILES; do
  out_name="out_$(basename $f)"
  $COMPILER $f > "$out_name.wat" && wat2wasm $out_name && wasm-interp --411 "$out_name.wasm"

  echo -e "------------------------------------\n"
done
echo "done!"
