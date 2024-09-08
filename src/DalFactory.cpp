#include "conffwk/DalFactory.hpp"

#include "conffwk/DalRegistry.hpp"
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
DalFactory::make(conffwk::DalRegistry& db, conffwk::ConfigObject& o) {


  TLOG() << "Building object " << o.UID() << " of class " << o.class_name();

  auto it = m_instantiators_2g.find(o.class_name());
  if (it == m_instantiators_2g.end()) {
    throw std::runtime_error("XXXXXXXXX");
  }

  auto dal_obj =  it->second(db,o);
  TLOG() << "Object " << o.UID() << " of class " << o.class_name() << " created " << (void*)dal_obj;

  return dal_obj;
  
}        

} // namespace conffwk   
} // namespace dunedaq
