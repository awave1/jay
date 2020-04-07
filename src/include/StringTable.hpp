#ifndef STRING_TABLE_HPP
#define STRING_TABLE_HPP

#include <map>
#include <string>

struct entry_t {
  unsigned int offset;
  std::size_t length;
};

class StringTable {
public:
  /**
   * @brief Insert a string into a table. Additionally, the method will
   * calculate the required offset for generated WASM code and string length,
   * which is required when calling prints routine.
   *
   * @param str string to be inserted into the table
   */
  void define(std::string str);

  /**
   * @brief Generate WASM code with all the strings in the source code
   *
   * @return std::string generated WASM code
   */
  std::string build_wasm_code();

  friend std::ostream &operator<<(std::ostream &os,
                                  const StringTable &str_table);

private:
  std::map<std::string, entry_t> table;
};

#endif /* STRING_TABLE_HPP */