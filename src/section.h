#pragma once

#include "defs.h"
#include "utils.h"
#include <cstdint>
#include <iostream>
#include <vector>

class SectionBase {
public:
  Elf32_Shdr *header;
  virtual size_t size_in_bytes() const = 0;
  virtual void serialize(std::ostream &, byte_order bo) = 0;
};

template <typename T> class Section : public SectionBase {
public:
  size_t offset;

  /* returns the size of the section in bytes */
  size_t size_in_bytes() const { return data.size() * sizeof(T); }

  /* gets the index of the last entry */
  size_t last_index() const {
    if (data.size() > 0) {
      return data.size() - 1;
    }

    return 0;
  }

  /* gets the (would be) index of the next entry */
  size_t next_index() const { return data.size(); }

  /* @brief: get entry at index
   * @param size_t idx the index of the entry
   * @returns uint32_t the entry at that index
   */
  bool get_entry(size_t idx, T &val) {
    if (idx >= data.size() || idx < 0)
      return false;

    val = data[idx];
    return true;
  }

  // should be deprecated in favor of push
  bool update_entry(size_t idx, T val) {
    if (idx >= data.size() || idx < 0) {
      return false;
    }
    data[idx] = val;
    return true;
  }

  /* at the end */
  bool push(T entry) {
    data.push_back(entry);
    header->sh_size += sizeof(entry);
  }

  /* at a specific location */
  bool insert(size_t idx, T value) { return update_entry(idx, value); }

  // void serialize(std::ostream& os, byte_order bo)
  //{
  //     for (T t: data)
  //     {
  //        t.serialize(os, bo);
  //     }
  // }
private:
  std::vector<T> data;
};
