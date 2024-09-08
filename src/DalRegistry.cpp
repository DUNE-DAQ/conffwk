#include "conffwk/Schema.hpp"
#include "conffwk/DalObject2g.hpp"

namespace dunedaq {
namespace conffwk {

DalRegistry::DalRegistry( conffwk::Configuration& confdb ) : 
  m_confdb(confdb) {

  // update_class_domain_map();

}

std::deque<std::set<std::string>>
DalRegistry::find_class_domains()
{
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
DalRegistry::update_class_domain_map()
{

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

} // namespace conffwk
} // namespace dunedaq
