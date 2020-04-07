#include "StringTable.hpp"

void StringTable::define(std::string str) {
  table.insert(table.begin(), std::pair<std::string, entry_t>(
                                  str, {offset_counter, str.length()}));
  offset_counter += str.length();
}

/**
 * @brief Generate WASM code with all the strings in the source code
 *
 * @return std::string generated WASM code
 */
std::string StringTable::build_wasm_code() {
  std::string code = "";
  for (auto const &[str, str_entry] : table) {
    code += "(data 0 (i32.const " + std::to_string(str_entry.offset) + ") \"" +
            str + "\")";
    code += "\n";
  }

  return code;
}
