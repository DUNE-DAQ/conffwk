#ifndef __DUNEDAQ_CONFFWK_DALREGISTRY_HPP__
#define __DUNEDAQ_CONFFWK_DALREGISTRY_HPP__

#include "conffwk/map.hpp"
#include "conffwk/Errors.hpp"

namespace dunedaq {
namespace conffwk {

class DalObject2g;
class Configuration;

class DalRegistry
{

  friend class DalObject2g;

public:
  DalRegistry(Configuration& confdb);

  Configuration& configuration() { return m_confdb; }

  template<class T> T *
  get(ConfigObject& obj, bool init_children=false, bool init_object=true);

  template<class T> T *
  get(const std::string& name, bool init_children=false, bool init_object=true, unsigned long rlevel = 0, const std::vector<std::string> * rclasses = nullptr);


  template<class T> T *
  find(const std::string & id);

  template<class T>
  const T* _ref(ConfigObject& obj, const std::string& name, bool read_children);

  template<class T> 
  void _ref(ConfigObject& obj, const std::string& name, std::vector<const T*>& results, bool read_children);

  void update_class_domain_map();

private:

  Configuration& m_confdb;

  mutable std::mutex m_mutex; // mutex used to access template objects (i.e. generated DAL)

  std::deque<std::set<std::string>> find_class_domains();

  conffwk::fmap<uint> m_class_domain_map;

  std::unordered_map<uint,conffwk::map<DalObject2g*>> m_cache;

};

} // namespace conffwk
} // namespace dunedaq

#endif /* __DUNEDAQ_CONFFWK_DALREGISTRY_HPP__ */
