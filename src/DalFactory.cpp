#include "conffwk/DalFactory.hpp"

#include "ers/ers.hpp"
#include "ers/internal/SingletonCreator.hpp"

#include "conffwk/Configuration.hpp"
#include "conffwk/ConfigObject.hpp"
#include "conffwk/Schema.hpp"
#include "conffwk/Errors.hpp"

#include <dlfcn.h>

namespace dunedaq {
namespace conffwk {

const std::string&
DalFactory::get_known_class_name_ref(const std::string& name) 
{
  std::lock_guard<std::mutex> scoped_lock(m_known_class_mutex);
  return *m_known_classes.emplace(name).first;
}

bool 
DalFactory::try_load_class_library(Configuration& db, const std::string& class_name) {

  auto& c = db.get_class_info(class_name);
  // fmt::print("- {} '{}'\n", class_name, c.p_schema_path);

  std::string file = c.p_schema_path;
  std::string search {"/schema/"};
  auto start = file.rfind(search) + search.size();
  auto end = file.find("/", start);

  std::string package = file.substr(start,end-start);
  std::string library = "lib"+package+".so";
  // fmt::print("{} -> {}\n", package, library);
  TLOG() << "Loading library " << library << " for class " << class_name;

  auto handle = dlopen(library.c_str(), RTLD_LAZY|RTLD_GLOBAL);
  if (handle == nullptr) {
    // fmt::print("Failed to load {}\n", library);

    throw (LoadDalFailed(ERS_HERE, library));
  }

  return handle != nullptr;
}

/**
 * \brief Create a new DaqOnject2g
 */
conffwk::DalObject* 
DalFactory::make(conffwk::DalRegistry& reg, conffwk::ConfigObject& o, bool upcast_unregistered) {


  TLOG_DEBUG(50) << "Building object " << o.UID() << " of class " << o.class_name();

  auto it = m_creators.find(o.class_name());

  if (it == m_creators.end()) {
    TLOG_DEBUG(50) << "Constructor for class " << o.class_name() << " not found";

    if (!this->try_load_class_library(reg.configuration(), o.class_name())) {
      throw NotFound(ERS_HERE, "class", o.class_name().c_str());
    }
    it = m_creators.find(o.class_name());
    if (it == m_creators.end()) {
        throw dunedaq::conffwk::NotFound(ERS_HERE, "class", o.class_name().c_str());
    }
  } else {
    TLOG() << ">>> Constructor for class " << o.class_name() << " found";
  }

  auto dal_obj = it->second(reg,o);
  TLOG_DEBUG(50) << "Object " << o.UID() << " of class " << o.class_name() << " created " << (void*)dal_obj;

  return dal_obj;
  
}

/**
 * \brief Create a new DaqOnject2g
 */
conffwk::DalObject* 
DalFactory::make(conffwk::DalRegistry& reg, conffwk::ConfigObject& o, const std::string& fallback_unregistred) {


  TLOG_DEBUG(50) << "Building object " << o.UID() << " of class " << o.class_name();

  auto it = m_creators.find(o.class_name());

  if (it == m_creators.end()) {
    if (!this->try_load_class_library(reg.configuration(), o.class_name())) {
      throw NotFound(ERS_HERE, "class", o.class_name().c_str());
    }
    it = m_creators.find(o.class_name());
    if (it == m_creators.end()) {
        throw dunedaq::conffwk::NotFound(ERS_HERE, "class", o.class_name().c_str());
    }

  }

  auto dal_obj = it->second(reg,o);
  TLOG_DEBUG(50) << "Object " << o.UID() << " of class " << o.class_name() << " created " << (void*)dal_obj;

  return dal_obj;
  
}    

DalFactory &
DalFactory::instance()
{
  static DalFactory * instance = ers::SingletonCreator<DalFactory>::create();
  return *instance;
}


DalObject *
DalFactory::get(Configuration& db, ConfigObject& obj, const std::string& uid, bool upcast_unregistered) const
{
  return (*instance().functions(db, obj.class_name(), upcast_unregistered).m_creator_fn)(db, obj, uid);
}

DalObject *
DalFactory::get(Configuration& db, ConfigObject& obj, const std::string& uid, const std::string& class_name) const
{
  return (*instance().functions(db, class_name, false).m_creator_fn)(db, obj, uid);
}


const DalFactoryFunctions&
DalFactory::functions(Configuration& db, const std::string& name, bool upcast_unregistered)
{
  auto it = m_classes.find(name);

  if (it == m_classes.end())
    {
      if (!this->try_load_class_library(db, name)) {
        throw NotFound(ERS_HERE, "class", name.c_str());
      }
      it = m_classes.find(name);
      if (it == m_classes.end()) {
          throw dunedaq::conffwk::NotFound(ERS_HERE, "class", name.c_str());
      }
  
    }

  return it->second;
}

const std::string&
DalFactory::class4algo(Configuration& db, const std::string& name, const std::string& algorithm) const
{
  for (const auto& x : m_classes)
    if (x.second.m_algorithms.find(algorithm) != x.second.m_algorithms.end() && db.try_cast(x.first, name))
      return x.first;

  static const std::string empty;
  return empty;
}


const DalFactoryFunctions&
DalFactory::functions(const std::string& name) const
{
  auto it = m_classes.find(name);

  ERS_ASSERT_MSG( (it != m_classes.end()), "writer lock was not initialized" );

  return it->second;
}


} // namespace conffwk   
} // namespace dunedaq
