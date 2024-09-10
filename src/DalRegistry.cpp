#include "conffwk/Schema.hpp"
#include "conffwk/DalObject2g.hpp"

namespace dunedaq {
namespace conffwk {

DalRegistry::DalRegistry( conffwk::Configuration& confdb ) : 
  m_confdb(confdb) {
}

DalRegistry::~DalRegistry() {
}

void
DalRegistry::clear() {
  for( const auto& [index, domain] : m_cache_domains ) {

    for(const auto& [uid, ptr] : domain.cache ) {
      delete ptr;
    }
  }

  m_class_domain_map.clear();
}

std::deque<std::set<std::string>>
DalRegistry::find_class_domains() {
  
  std::deque<std::set<std::string>> domains;

  std::deque<dunedaq::conffwk::class_t> seeds;
  for (const auto& c : m_confdb.get_class_list()) {
    auto ci = m_confdb.get_class_info(c);
    if (ci.p_superclasses.empty())
      seeds.push_back(ci);
  }

  for (const auto& ci : seeds) {
    // Make a candidate domain based using the seed subclasses
    std::set<std::string> class_domain;
    class_domain.insert(ci.p_name);
    class_domain.insert(ci.p_subclasses.begin(), ci.p_subclasses.end());

    // Look for overlaps with other domains
    std::deque<std::set<std::string>> overlapping;
    for (auto& d : domains) {
      std::set<std::string> intersection;
      std::set_intersection(d.begin(), d.end(), class_domain.begin(), class_domain.end(), std::inserter(intersection, intersection.begin()));
      // non-zero intersection, overlap found
      if (intersection.size() > 0) {
        overlapping.push_back(d);
      }
    }

    // If overlapping are found, add all overlapping to
    // the new domain and remove them from the domain list
    if (!overlapping.empty()) {
      for (auto& d : overlapping) {
        // merge the existing cluster in class_domain
        class_domain.insert(d.begin(), d.end());
        // Remove the old cluster from the list
        auto it = std::find(domains.begin(), domains.end(), d);
        if (it != domains.end()) {
          domains.erase(it);
        }
      }
    }

    domains.push_back(class_domain);
  }

  return domains;
}

void
DalRegistry::update_class_domain_map() {

  m_class_domain_map.clear();

  auto domains = this->find_class_domains();
  for (size_t i(0); i < domains.size(); ++i) {
    const auto& dom = domains[i];
    for (const auto& class_name : dom) {
      m_class_domain_map[&conffwk::DalFactory::instance().get_known_class_name_ref(class_name)] = i;
      std::cout << " - " << class_name << " : " <<  i << std::endl;
    }
  }
}


void
DalRegistry::update(
            const std::string& class_name,
            const std::vector<std::string>& modified,
            const std::vector<std::string>& removed,
            const std::vector<std::string>& created) {
  
  // Find the class domain of T
  auto it_dom = m_class_domain_map.find(&DalFactory::instance().get_known_class_name_ref(class_name));

  // Class not known, this should not happen
  if ( it_dom == m_class_domain_map.end() ) {
    throw dunedaq::conffwk::NotFound(ERS_HERE, class_name.c_str(), "<None>");
  }

  // get the correct cache domain
  auto& domain = m_cache_domains[it_dom->second];

  for( const auto& [uid, ptr] : domain.cache ) {

    // Check if the ptr class is derived from class_name
    m_confdb.is_subclass_of(ptr->class_name(), class_name);
    
    if (!ptr)
      continue;

    bool update = (
      ( std::find(modified.begin(), modified.end(), ptr->UID()) != modified.end() ) or
      ( std::find(removed.begin(), removed.end(), ptr->UID()) != removed.end() ) or
      ( std::find(created.begin(), created.end(), ptr->UID()) != created.end() )
    );

    if (!update)
      continue;

    std::lock_guard<std::mutex> scoped_lock(domain.mutex);
    ptr->p_was_read = false;
  }
}

void
DalRegistry::unread_all() {

  for( const auto& [dom_id, domain] : m_cache_domains ) {
  
    std::lock_guard<std::mutex> scoped_lock(domain.mutex);

    for( const auto& [id, ptr] : domain.cache ) {
      ptr->p_was_read = false;
    }

  }
}


void
DalRegistry::_rename_object(std::string class_name, std::string old_id, std::string new_id) {
  // Find the class domain of T
  auto it_dom = m_class_domain_map.find(&DalFactory::instance().get_known_class_name_ref(class_name));

  // Class not known, this should not happen
  if ( it_dom == m_class_domain_map.end() ) {
    throw dunedaq::conffwk::NotFound(ERS_HERE, class_name.c_str(), old_id.c_str());
  }

  auto& domain = m_cache_domains[it_dom->second];

  auto it = domain.cache.find(old_id);
  if (it == domain.cache.end())
    return;

  domain.cache[new_id] = it->second;
  domain.cache.erase(it);

  std::lock_guard<std::mutex> scoped_lock(it->second->m_mutex);
  it->second->p_UID = new_id;
  
}


} // namespace conffwk
} // namespace dunedaq
