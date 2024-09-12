#include "conffwk/DalFactory.hpp"

#include "ers/ers.hpp"
#include "ers/internal/SingletonCreator.hpp"

#include "conffwk/Configuration.hpp"
#include "conffwk/ConfigObject.hpp"

namespace dunedaq {
namespace conffwk {

const std::string&
DalFactory::get_known_class_name_ref(const std::string& name) 
{
  std::lock_guard<std::mutex> scoped_lock(m_known_class_mutex);
  return *m_known_classes.emplace(name).first;
}

/**
 * \brief Create a new DaqOnject2g
 */
conffwk::DalObject2g* 
DalFactory::make(conffwk::DalRegistry& db, conffwk::ConfigObject& o, bool upcast_unregistered) {


  TLOG() << "EEE Building object " << o.UID() << " of class " << o.class_name();

  auto it = m_instantiators_2g.find(o.class_name());

  if (it == m_instantiators_2g.end()) {
    TLOG() << "EEE Constructor for class " << o.class_name() << " not found";
    if (upcast_unregistered) {

      auto sups = db.configuration().superclasses().find(&o.class_name());
      if (sups != db.configuration().superclasses().end()) {
        for (auto c : sups->second) {
          it = m_instantiators_2g.find(*c);
          if (it != m_instantiators_2g.end()) {
            TLOG_DEBUG(1) << "use first suitable base class " << c << " instead of unregistered DAL class " << o.class_name();
            auto dal_obj = it->second(db,o);
            TLOG() << "Object " << o.UID() << " of class " << o.class_name() << " created " << (void*)dal_obj;

            return dal_obj;
          }
        }
      }
      
    }
    throw dunedaq::conffwk::NotFound(ERS_HERE, "class", o.class_name().c_str());
  }

  auto dal_obj = it->second(db,o);
  TLOG() << "Object " << o.UID() << " of class " << o.class_name() << " created " << (void*)dal_obj;

  return dal_obj;
  
}

/**
 * \brief Create a new DaqOnject2g
 */
conffwk::DalObject2g* 
DalFactory::make(conffwk::DalRegistry& db, conffwk::ConfigObject& o, const std::string& fallback_unregistred) {


  TLOG() << "EEE Building object " << o.UID() << " of class " << o.class_name();

  auto it = m_instantiators_2g.find(o.class_name());

  if (it == m_instantiators_2g.end()) {
    TLOG() << "EEE Constructor for class " << o.class_name() << " not found";
    if (!fallback_unregistred.empty()) {
      it = m_instantiators_2g.find(fallback_unregistred); 
      if (it == m_instantiators_2g.end()) {
        throw dunedaq::conffwk::NotFound(ERS_HERE, "class", o.class_name().c_str());
      }
      TLOG_DEBUG(1) << "use first suitable base class " << fallback_unregistred << " instead of unregistered DAL class " << o.class_name();
      auto dal_obj = it->second(db,o);
      TLOG() << "Object " << o.UID() << " of class " << fallback_unregistred << " created " << (void*)dal_obj;

      return dal_obj;
    }
    throw dunedaq::conffwk::NotFound(ERS_HERE, "class", o.class_name().c_str());
  }

  auto dal_obj = it->second(db,o);
  TLOG() << "Object " << o.UID() << " of class " << o.class_name() << " created " << (void*)dal_obj;

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
DalFactory::functions(const Configuration& db, const std::string& name, bool upcast_unregistered) const
{
  auto it = m_classes.find(name);

  if (it == m_classes.end())
    {
      if (upcast_unregistered)
        {
          auto x = db.superclasses().find(&name);
          if (x != db.superclasses().end())
            {
              for (auto c : x->second)
                {
                  auto sc = m_classes.find(*c);
                  if (sc != m_classes.end())
                    {
                      TLOG_DEBUG(1) << "use first suitable base class " << c << " instead of unregistered DAL class " << name;
                      return sc->second;
                    }
                }
            }
        }

      std::string text(std::string("DAL class ") + name + " was not registered");
      throw dunedaq::conffwk::Generic(ERS_HERE, text.c_str());
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
