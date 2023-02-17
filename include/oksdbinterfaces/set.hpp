#ifndef OKSDB_INTERFACE_SET_H_
#define OKSDB_INTERFACE_SET_H_

#include "oksdbinterfaces/string_ptr.hpp"
#include <unordered_set>

namespace oksdbinterfaces
{
  typedef std::unordered_set<std::string> set;

  // compare string pointers (not values!)
  typedef std::unordered_set<const std::string *, string_ptr_hash> fset;
}

#endif // OKSDB_INTERFACE_SET_H_
