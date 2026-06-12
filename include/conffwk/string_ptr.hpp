/*
 * DUNE DAQ modification notice:
 * This file has been modified from the original ATLAS config source for the DUNE DAQ project.
 * Fork baseline commit: 67a24e731 (2022-10-27).
 * Renamed since fork: yes (from config/string_ptr.h to include/conffwk/string_ptr.hpp).
 */

#ifndef CONFFWK_STRING_PTR_H_
#define CONFFWK_STRING_PTR_H_

#include <string>

namespace dunedaq {
namespace conffwk
{
  struct string_ptr_compare {
    bool operator()(const std::string * left, const std::string * right) const
    {
      return *left < *right;
    }
  };

  struct string_ptr_hash
  {
    inline size_t operator() ( const std::string * x ) const {
      return reinterpret_cast<size_t>(x);
    }
  };
} // namespace conffwk
} // namespace dunedaq
#endif // CONFFWK_STRING_PTR_H_
