/**
 * @file StringTable.cpp
 * @author Artem Golovin (30018900)
 * @brief Generate table of strings to append to WAT code
 */

#include "StringTable.hpp"

/**
 * @brief Insert a string into a table. Additionally, the method will
 * calculate the required offset for generated WASM code and string length,
 * which is required when calling prints routine.
 *
 * @param str string to be inserted into the table
 */
void StringTable::define(std::string str) {
  table.insert(table.begin(), std::pair<std::string, entry_t>(
                                  str, {offset_counter, str.length()}));
  offset_counter += str.length();
}

/**
 * @brief lookup a string in a table
 *
 * @param str string to look for
 * @return entry_t entry for a string
 */
entry_t StringTable::lookup(std::string str) { return table.at(str); }

/**
 * @brief Generate WASM code with all the strings in the source code
 *
 * @return std::string generated WASM code
 */
std::string StringTable::build_wasm_code() {
  std::string code = "";

  // sort the strings by their offset value
  auto comporator = [](std::pair<std::string, entry_t> el1,
                       std::pair<std::string, entry_t> el2) {
    return el1.second.offset < el2.second.offset;
  };

  std::set<std::pair<std::string, entry_t>, comporator_t> sorted_map(
      table.begin(), table.end(), comporator);

  for (auto const &[str, str_entry] : sorted_map) {
    code += "  ";
    code += "(data 0 (i32.const " + std::to_string(str_entry.offset) + ") \"" +
            str + "\")";
    code += "\n";
  }

  return code;
}