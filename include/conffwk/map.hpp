#ifndef CONFFWK_MAP_H_
#define CONFFWK_MAP_H_

#include "conffwk/string_ptr.hpp"
#include <unordered_map>

namespace dunedaq {
namespace conffwk
{
  template<class T>
    class map : public std::unordered_map<std::string, T>
    {
    public:
      map()
      {
        ;
      }
    };

  template<class T>
    class multimap : public std::unordered_multimap<std::string, T>
    {
    public:
      multimap()
      {
        ;
      }
    };

  // compare pointers by string value
  template<class T>
    class pmap : public std::map<const std::string *, T, string_ptr_compare>
    {
    public:
      pmap()
      {
        ;
      }
    };

  // compare string pointers (not values!)
  template<class T>
    class fmap : public std::unordered_map<const std::string *, T, string_ptr_hash>
    {
    public:
      fmap()
      {
        ;
      }
    };
} // namespace conffwk
} // namespace dunedaq

#endif // CONFFWK_MAP_H_
