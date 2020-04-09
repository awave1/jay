#ifndef STRING_TABLE_HPP
#define STRING_TABLE_HPP

#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <string>

struct entry_t {
  unsigned int offset;
  std::size_t length;
};

typedef std::function<bool(std::pair<std::string, entry_t>,
                           std::pair<std::string, entry_t>)>
    comporator_t;

class StringTable {
public:
  StringTable() {
    define("true");
    define("false");
  }

  /**
   * @brief Insert a string into a table. Additionally, the method will
   * calculate the required offset for generated WASM code and string length,
   * which is required when calling prints routine.
   *
   * @param str string to be inserted into the table
   */
  void define(std::string str);

  /**
   * @brief
   *
   * @param str
   * @return entry_t
   */
  entry_t lookup(std::string str);

  /**
   * @brief Generate WASM code with all the strings in the source code
   *
   * @param indentation
   * @return std::string generated WASM code
   */

  std::string build_wasm_code(std::string indentation);

  friend std::ostream &operator<<(std::ostream &os,
                                  const StringTable &str_table);

private:
  unsigned int offset_counter = 0;
  std::map<std::string, entry_t> table;
};

#endif /* STRING_TABLE_HPP */