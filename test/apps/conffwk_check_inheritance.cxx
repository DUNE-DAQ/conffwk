#include "CLI/CLI.hpp"
#include <fmt/core.h>
#include <fmt/ranges.h>

#include "conffwk/Configuration.hpp"
#include "conffwk/DalFactory.hpp"
#include "conffwk/Schema.hpp"

std::deque<std::set<std::string>>
construct_class_domains(dunedaq::conffwk::Configuration& cfg_db)
{

  std::deque<std::set<std::string>> domains;

  std::deque<dunedaq::conffwk::class_t> progenitors;
  for (const auto& c : cfg_db.get_class_list()) {
    auto ci = cfg_db.get_class_info(c);
    if (ci.p_superclasses.empty())
      progenitors.push_back(ci);
    // fmt::print("   - {} {}\n", c, ci.p_superclasses.size(), ci.p_superclasses.size());
  }

  for (const auto& ci : progenitors) {
    // Make a candidate cluster based using the seed subclasses
    std::set<std::string> class_cluster;
    class_cluster.insert(ci.p_name);
    class_cluster.insert(ci.p_subclasses.begin(), ci.p_subclasses.end());

    // Look for overlaps with other domains
    std::deque<std::set<std::string>> overlapping;
    for (auto& dom : domains) {
      std::set<std::string> intersection;
      std::set_intersection(dom.begin(), dom.end(), class_cluster.begin(), class_cluster.end(), std::inserter(intersection, intersection.begin()));
      // non-zero intersection, overlap found
      if (intersection.size() > 0) {
        overlapping.push_back(dom);
      }
    }

    // If overlapping are found, merge all overlapping domains
    if ( !overlapping.empty() ) {
      for( auto& dom : overlapping ) {
        // merge the existing cluster in class_cluster
        class_cluster.insert(dom.begin(), dom.end());
        // Remove the old cluster from the list
        auto it = std::find(domains.begin(), domains.end(), dom);
        if (it!= domains.end()) {
            domains.erase(it);
        }
      }
    }


    domains.push_back(class_cluster);
  }

  return domains;
}

int
main(int argc, char const* argv[])
{
  using namespace dunedaq;

  CLI::App app{ "App description" };

  std::string file;
  app.add_option("-f,--file", file, "Schema file")->required()->check(CLI::ExistingFile);

  CLI11_PARSE(app, argc, argv);

  fmt::print("Configuration database file: {}\n", file);

  conffwk::Configuration db("oksconflibs:" + file);

  std::deque<std::set<std::string>> domains = construct_class_domains(db);


  std::map<std::string, uint> class_domain_map;
  conffwk::fmap<uint> class_domain_map_2;
  // Print the clustered domains
  fmt::print("Found {} inheritance domains\n", domains.size());
  for( size_t i(0); i<domains.size(); ++i ) {
    const auto& dom = domains[i];
    // fmt::print("   - {} : {}", i, fmt::join(dom, ","));
    fmt::print("   - {} : {}", i, dom);
    for( const auto& class_name : dom ) {
      class_domain_map[class_name] = i;
      class_domain_map_2[&conffwk::DalFactory::instance().get_known_class_name_ref(class_name)] = i;
    }
    fmt::print("\n");
  }

  for( const auto& [name, id] : class_domain_map ) {
    fmt::print("- {} : {}\n", name, id);
  }

  fmt::print("-------------------\n");
  fmt::print("cdm2 {}\n", class_domain_map_2.size());
  


  for( const auto& [name, id] : class_domain_map_2 ) {
    fmt::print("+ {} : {}\n", *name, id);
  }

  /* code */
  return 0;
}
