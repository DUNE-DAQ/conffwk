/*
 * DUNE DAQ modification notice:
 * This file has been modified from the original ATLAS config source for the DUNE DAQ project.
 * Fork baseline commit: 67a24e731 (2022-10-27).
 * Renamed since fork: yes (from config/set.h to include/conffwk/set.hpp).
 */

#ifndef CONFFWK_SET_H_
#define CONFFWK_SET_H_

#include "conffwk/string_ptr.hpp"
#include <unordered_set>

namespace dunedaq {
namespace conffwk
{
  typedef std::unordered_set<std::string> set;

  // compare string pointers (not values!)
  typedef std::unordered_set<const std::string *, string_ptr_hash> fset;
} // namespace conffwk
} // namespace dunedaq


#endif // CONFFWK_SET_H_
