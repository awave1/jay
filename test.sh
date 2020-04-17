#!/usr/bin/env bash

# Run the compiler against testfiles in codegen

TEST_FILES=$(find "$PWD/test/codegen" -name "*.*")
COMPILER=$PWD/jay
PROF_BIN="/home/profs/aycock/411/bin"

WATWASM=wat2wasm
WASMINTERP=wasm-interp

if ! type "wat2wasm" > /dev/null; then
  WATWASM=$PROF_BIN/$WATWASM
fi

if ! type "$WASMINTERP" > /dev/null; then
  WASMINTERP=$PROF_BIN/$WASMINTERP
fi

echo -e "running tests...\n"
echo "  > using $WATWASM"
echo "  > using $WASMINTERP"

for f in $TEST_FILES; do
  echo "testing: $f"
  out_name="out_$(basename $f)"
  $COMPILER $f > "$PWD/$out_name.wat"
  $WATWASM "$PWD/$out_name.wat" && $WASMINTERP --411 "$PWD/$out_name.wasm"

  echo -e "------------------------------------\n"
done
echo "done!"
