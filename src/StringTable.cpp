#include "StringTable.hpp"

void StringTable::define(std::string str) {}

/**
 * @brief Generate WASM code with all the strings in the source code
 *
 * @return std::string generated WASM code
 */
std::string StringTable::build_wasm_code() {
  std::string code = "";
  for (auto const &[str, str_entry] : table) {
    // (data 0 (i32.const $offset) "str")
    code += "(data 0 (i32.const " + std::to_string(str_entry.offset) + ") \"" +
            str + "\")";
    code += "\n";
  }

  return code;
}
