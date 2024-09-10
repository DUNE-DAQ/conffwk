#ifndef __DUNEDAQ_CONFFWK_DALREGISTRY_HXX__
#define __DUNEDAQ_CONFFWK_DALREGISTRY_HXX__


namespace dunedaq {
namespace conffwk {

template<class T> T *
DalRegistry::get(ConfigObject& obj, bool init_children, bool init_object) {

  if ( !m_confdb.try_cast(T::s_class_name, obj.class_name()) ) {
    return nullptr;
  }

  // Find the class domain of T
  auto it_dom = m_class_domain_map.find(&DalFactory::instance().get_known_class_name_ref(T::s_class_name));
  
  // Class not known, this should not happen
  if ( it_dom == m_class_domain_map.end() ) {
    throw dunedaq::conffwk::NotFound(ERS_HERE, T::s_class_name.c_str(), "<None>");
  }

  auto& domain = m_cache_domains[it_dom->second];

  DalObject2g*& dal_ptr(domain.cache[obj.m_impl->m_id]);
  T* result = dynamic_cast<T*>(dal_ptr);

  if (result == nullptr) {
    // result = new T(*this, obj);
    dal_ptr = DalFactory::instance().make(*this, obj);

    TLOG() << "Casting " << obj.UID() << " of class " << obj.class_name() << " to " << T::s_class_name;
    result = dynamic_cast<T*>(dal_ptr);
    TLOG() << "From " << (void*)dal_ptr << " to " << (void*)result;

    if (init_object) {
      std::lock_guard<std::mutex> scoped_lock(result->m_mutex);
      result->init(init_children);
    }

  } else if (obj.m_impl != result->p_obj.m_impl) {

    std::lock_guard<std::mutex> scoped_lock(result->m_mutex);
    result->set(obj); // update implementation object; to be used in case if the object is re-created
  
  }

  return result;   
}

template<class T> T *
DalRegistry::get(const std::string& name, bool init_children, bool init_object, unsigned long rlevel, const std::vector<std::string> * rclasses) {

  // Find the class domain of T
  auto it_dom = m_class_domain_map.find(&DalFactory::instance().get_known_class_name_ref(T::s_class_name));

  // Class not known, this should not happen
  if ( it_dom == m_class_domain_map.end() ) {
    throw dunedaq::conffwk::NotFound(ERS_HERE, T::s_class_name, name);
  }

  auto& domain = m_cache_domains[it_dom->second];

  auto it_ptr = domain.cache.find(name);

  if ( it_ptr == domain.cache.end()) {
    try {
      conffwk::ConfigObject obj;
      m_confdb._get(T::s_class_name, name, obj, rlevel, rclasses);

      DalObject2g*& dal_ptr(domain.cache[obj.m_impl->m_id]);
      T* result = dynamic_cast<T*>(dal_ptr);
      if (result == nullptr) {
        // result = new T(*this, obj);
        dal_ptr = DalFactory::instance().make(*this, obj);

        TLOG() << "Casting " << obj.UID() << " of class " << obj.class_name() << " to " << T::s_class_name;
        result = dynamic_cast<T*>(dal_ptr);
        TLOG() << "From " << (void*)dal_ptr << " to " << (void*)result;

        if (init_object) {
          std::lock_guard<std::mutex> scoped_lock(result->m_mutex);
          result->init(init_children);
        }
      } else if (obj.m_impl != result->p_obj.m_impl) {
        std::lock_guard<std::mutex> scoped_lock(result->m_mutex);
        result->set(obj); // update implementation object; to be used in case if the object is re-created
      }
      
      std::cout << "Returning " << (void*)result << std::endl;
      return result;

    } catch (dunedaq::conffwk::NotFound& ex) {
      if (!strcmp(ex.get_type(), "class")) {
        std::ostringstream text;
        text << "wrong database schema, cannot find class \"" << ex.get_data() << '\"';
        throw dunedaq::conffwk::Generic(ERS_HERE, text.str().c_str());
      }
    }
  }
 
  std::cout << "Found " << (void*)it_ptr->second << std::endl;
  return dynamic_cast<T*>(it_ptr->second);

}


template<class T>
T *
DalRegistry::find(const std::string & id) {
  std::lock_guard<std::mutex> scoped_lock(m_mutex);

  auto it_dom = m_class_domain_map.find(&T::s_class_name);

  if ( it_dom == m_class_domain_map.end()) {
    return nullptr;
  }

  auto& domain = m_cache_domains[it_dom.second];
  auto it_obj = domain.cache.find(id);
  return (it_obj != domain.cache.end() ? dynamic_cast<T*>(it_obj->second) : nullptr);
}

template<class T>
const T*
DalRegistry::_ref(ConfigObject& obj, const std::string& name, bool read_children)
{
  ConfigObject res;

  try {
    obj.get(name, res);
  } catch (dunedaq::conffwk::Generic& ex) {
    throw(dunedaq::conffwk::Generic(ERS_HERE, Configuration::mk_ref_ex_text("an object", T::s_class_name, name, obj).c_str(), ex));
  }

  return ((!res.is_null()) ? this->get<T>(res, read_children, read_children) : nullptr);
}


template<class T>
bool 
DalRegistry::is_valid(const T * object) noexcept {

  std::lock_guard<std::mutex> scoped_lock(m_mutex);

  auto it_dom = m_class_domain_map.find(&T::s_class_name);
  if ( it_dom == m_class_domain_map.end()) {
    return false;
  }

  for( const auto& [uid, ptr] : m_cache_domains[it_dom->second].cache ) {
    if ( ptr == object ) {
      return true;
    }
  }
}

template<class T>
void
DalRegistry::update(const std::vector<std::string>& modified,
            const std::vector<std::string>& removed,
            const std::vector<std::string>& created) {
  
  this->update(T::s_class_name, modified, removed, created);
}

template<class T> 
void
DalRegistry::_reset_objects() noexcept {

  // Find the class domain of T
  auto it_dom = m_class_domain_map.find(&DalFactory::instance().get_known_class_name_ref(T::s_class_name));

  // Class not known, this should not happen
  if ( it_dom == m_class_domain_map.end() ) {
    throw dunedaq::conffwk::NotFound(ERS_HERE, T::s_class_name.c_str(), "<None>");
  }

  auto& domain = m_cache_domains[it_dom->second];

  // Loop over the objects in the domain and reset those inheriting from T
  for( const auto& [uid, ptr] : domain.cache ) {
    

    T* obj_ptr = dynamic_cast<T*>(ptr);
    
    if (!obj_ptr)
      continue;

    std::lock_guard<std::mutex> scoped_lock(domain.mutex);
    obj_ptr->p_was_read = false;
  }
}

} // namespace conffwk
} // namespace dunedaq

#endif /*__DUNEDAQ_CONFFWK_DALREGISTRY_HXX__ */