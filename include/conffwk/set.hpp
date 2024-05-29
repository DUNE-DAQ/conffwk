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
