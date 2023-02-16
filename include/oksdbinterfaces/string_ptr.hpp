#ifndef OKSDB_INTERFACE_STRING_PTR_H_
#define OKSDB_INTERFACE_STRING_PTR_H_

#include <string>

namespace oksdbinterfaces
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
}

#endif // OKSDB_INTERFACE_STRING_PTR_H_
