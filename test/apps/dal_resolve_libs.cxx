#include "CLI/CLI.hpp"
#include <fmt/core.h>
#include <fmt/ranges.h>
#include <dlfcn.h>

#include "conffwk/Configuration.hpp"
#include "conffwk/DalFactory.hpp"
#include "conffwk/Schema.hpp"



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


    for ( const auto& class_name : db.get_class_list()) {

        auto& c = db.get_class_info(class_name);
        fmt::print("- {} '{}'\n", class_name, c.p_schema_path);

        std::string file = c.p_schema_path;
        std::string search {"/schema/"};
        auto start = file.rfind(search) + search.size();
        auto end = file.find("/", start);

        std::string package = file.substr(start,end-start);
        std::string library = "lib"+package+".so";
        fmt::print("Loading library {} from {}\n", library, package);

        auto handle = dlopen(library.c_str(), RTLD_LAZY|RTLD_GLOBAL);
        if (handle == nullptr) {
          fmt::print("Failed to load {}\n", library);

          // throw (LoadDalFailed(ERS_HERE, library));
        }

    }
}