#ifndef __DUNEDAQ_CONFFWK_CONFIGURATION_HXX__
#define __DUNEDAQ_CONFFWK_CONFIGURATION_HXX__

#include "conffwk/details/DalRegistry.hxx"

namespace dunedaq {
namespace conffwk {
  

template<class T>
  const T *
  Configuration::create(const std::string& at, const std::string& id, bool init_object)
  {
    ConfigObject obj;

    std::lock_guard<std::mutex> scoped_lock(m_tmpl_mutex);
    create(at, T::s_class_name, id, obj);
    return m_registry.get<T>(obj, false, init_object);
  }


template<class T>
  void
  Configuration::destroy(T& obj)
  {
    destroy_obj(const_cast<ConfigObject&>(obj.config_object()));
  }

// Get object of given class and instantiate the template parameter with it.
template<class T>
  const T *
  Configuration::_get(const std::string& name, bool init_children, bool init_object, unsigned long rlevel, const std::vector<std::string> * rclasses)
  {
    return m_registry.get<T>(name, init_children, init_object, rlevel, rclasses);
  }

// Instantiate the template parameter using existing conffwk object.
template<class T>
  const T *
  Configuration::_get(ConfigObject& obj, bool init_children, bool init_object)
  {
    return m_registry.get<T>(obj, init_children, init_object);
  }

// FIXME : not supported, delete
// template<class T>
//   const T *
//   Configuration::_get(ConfigObject& obj, const std::string& id)
//   {
//     return m_registry.get<T>(obj, id);
//   }

template<class T>
  const T *
  Configuration::_find(const std::string& id)
  {
    // auto it = m_cache_map.find(&T::s_class_name);
    // return (it != m_cache_map.end() ? static_cast<Cache<T>*>(it->second)->find(id) : nullptr);
    return m_registry.find<T>(id);
  }

// Get all objects the given class and instantiate a vector of the template parameters object with it.
template<class T>
  void
  Configuration::_get(std::vector<const T*>& result, bool init_children, bool init_object, const std::string& query, unsigned long rlevel, const std::vector<std::string> * rclasses)
  {
    std::vector<ConfigObject> objs;

    try
      {
        get(T::s_class_name, objs, query, rlevel, rclasses);
      }
    catch (dunedaq::conffwk::NotFound & ex)
      {
        std::ostringstream text;
        text << "wrong database schema, cannot find class \'" << ex.get_data() << '\'';
        throw dunedaq::conffwk::Generic(ERS_HERE, text.str().c_str());
      }

      // FIXME
      assert(false);

    if (!objs.empty())
      {
        for (auto& i : objs)
          {
            result.push_back(this->_get<T>(i, init_children, init_object));
          }
      }
  }


// Get relation from object and instantiate result with it.
template<class T>
  const T *
  Configuration::_ref(ConfigObject& obj, const std::string& name, bool read_children)
  {
    return m_registry._ref<T>(obj, name, read_children);
    // ConfigObject res;

    // try
    //   {
    //     obj.get(name, res);
    //   }
    // catch (dunedaq::conffwk::Generic & ex)
    //   {
    //     throw(dunedaq::conffwk::Generic( ERS_HERE, mk_ref_ex_text("an object", T::s_class_name, name, obj).c_str(), ex ) );
    //   }

    // return ((!res.is_null()) ? get_cache<T>(res.class_name())->get(*this, res, read_children, read_children) : nullptr);
  }


// Get multiple relations from object and instantiate result with it.
template<class T>
  void
  Configuration::_ref(ConfigObject& obj, const std::string& name, std::vector<const T*>& results, bool read_children)
  {

    return m_registry._ref<T>(obj, name, results, read_children);
    // std::vector<ConfigObject> objs;

    // results.clear();

    // try
    //   {
    //     obj.get(name, objs);
    //     results.reserve(objs.size());

    //     for (auto& i : objs)
    //       {
    //         results.push_back(get_cache<T>(i.class_name())->get(*this, i, read_children, read_children));
    //       }
    //   }
    // catch (dunedaq::conffwk::Generic & ex)
    //   {
    //     throw(dunedaq::conffwk::Generic( ERS_HERE, mk_ref_ex_text("objects", T::s_class_name, name, obj).c_str(), ex ) );
    //   }
  }

template<class T, class V>
  void
  Configuration::referenced_by(const T& obj, std::vector<const V*>& results, const std::string& relationship_name, bool check_composite_only, bool init, unsigned long rlevel, const std::vector<std::string> * rclasses)
  {
    std::vector<ConfigObject> objs;

    results.clear();

    std::lock_guard<std::mutex> scoped_lock(m_tmpl_mutex);

    try
      {
        obj.p_obj.referenced_by(objs, relationship_name, check_composite_only, rlevel, rclasses);

        for (auto& i : objs)
          {
            if (try_cast(V::s_class_name, i.class_name()) == true)
              {
                // if (const V * o = get_cache<V>()->get(*this, i, init, init))
                if (const V * o = m_registry.get<V>(i, init, init))
                  {
                    results.push_back(o);
                  }
              }
          }
      }
    catch (dunedaq::conffwk::Generic & ex)
      {
        throw(dunedaq::conffwk::Generic( ERS_HERE, mk_ref_by_ex_text(V::s_class_name, relationship_name, obj.p_obj).c_str(), ex ) );
      }
  }


// template<class T>
//   Configuration::Cache<T>::~Cache() noexcept
//   {
//     // delete each object in cache
//     for (const auto& i : m_cache)
//       {
//         delete i.second;
//       }
//   }


// template<class T>
//   T *
// Configuration::Cache<T>::get(Configuration& conffwk,
//                              ConfigObject& obj, bool init_children, bool init_object)
//   {
//     DalObject*& dalptr = m_cache[obj.m_impl->m_id];
//     T* result = dynamic_cast<T*>(dalptr);
//     if (dalptr == nullptr)
//       {
//         result = dynamic_cast<T*>(m_functions.m_instantiator_fn(conffwk, obj));
//         if (init_object)
//           {
//             std::lock_guard<std::mutex> scoped_lock(result->m_mutex);
//             result->init(init_children);
//           }
//         dalptr = result;
//       }
//     else if(obj.m_impl != result->p_obj.m_impl)
//       {
//         std::lock_guard<std::mutex> scoped_lock(result->m_mutex);
//         result->set(obj); // update implementation object; to be used in case if the object is re-created
//       }
//     increment_gets(conffwk);
//     return result;
//   }

// template<class T>
//   T *
//   Configuration::Cache<T>::find(const std::string& id)
//   {
//     auto it = m_cache.find(id);
//     return (it != m_cache.end() ? dynamic_cast<T*>(it->second) : nullptr);
//   }

// template<class T>
//   T *
//   Configuration::Cache<T>::get(Configuration& db, ConfigObject& obj, const std::string& id)
//   {

//     DalObject*& dalptr = m_cache[id];
//     T* result = dynamic_cast<T*>(dalptr);
//     if (dalptr == nullptr)
//       {
//         result = dynamic_cast<T*>(m_functions.m_instantiator_fn(db, obj));
//         if (id != obj.UID())
//           {
//             result->p_UID = id;
//             m_t_cache.emplace(obj.UID(), result);
//           }
//         dalptr = result;
//       }
//     else if(obj.m_impl != result->p_obj.m_impl)
//       {
//         std::lock_guard<std::mutex> scoped_lock(result->m_mutex);
//         result->set(obj); // update implementation object; to be used in case if the object is re-created
//       }
//     increment_gets(db);
//     return result;
//   }


//   // Get object from cache or create it.

// template<class T> T *
// Configuration::Cache<T>::get(Configuration& conffwk, const std::string& name, bool init_children, bool init_object, unsigned long rlevel, const std::vector<std::string> * rclasses)
// {

//   typename conffwk::map<DalObject*>::iterator i = m_cache.find(name);
//   if(i == m_cache.end()) {
//     try {
//       ConfigObject obj;
//       // class name should be the true cache class name
//       conffwk._get(m_class_name, name, obj, rlevel, rclasses);
//       return get(conffwk, obj, init_children, init_object);
//     }
//     catch(dunedaq::conffwk::NotFound & ex) {
//       if(!strcmp(ex.get_type(), "class")) {
//         std::ostringstream text;
// 	      text << "wrong database schema, cannot find class \"" << ex.get_data() << '\"';
// 	      throw dunedaq::conffwk::Generic(ERS_HERE, text.str().c_str());
//       }
//       else {
//         return 0;
//       }
//     }
//   }
//   increment_gets(conffwk);
//   return dynamic_cast<T*>(i->second);
// }


template<class T>
  bool
  Configuration::is_valid(const T * object) noexcept
  {
    // std::lock_guard<std::mutex> scoped_lock(m_tmpl_mutex);

    // auto j = m_cache_map.find(&T::s_class_name);

    // if (j != m_cache_map.end())
    //   {
    //     Cache<T> *c = static_cast<Cache<T>*>(j->second);

    //     for (const auto& i : c->m_cache)
    //       {
    //         if (i->second == object)
    //           return true;
    //       }
    //   }

    // return false;

    return m_registry.is_valid(object);
  }


template<class T> void
Configuration::update(const std::vector<std::string>& modified,
                      const std::vector<std::string>& removed,
                      const std::vector<std::string>& created) noexcept
  {
    // auto j = m_cache_map.find(&T::s_class_name);

    // TLOG_DEBUG(4) << "call for class \'" << T::s_class_name << '\'';

    // if (j != m_cache_map.end())
    //   {
    //     Cache<T> *c = static_cast<Cache<T>*>(j->second);
    //     set_cache_unread(removed, *c);
    //     set_cache_unread(created, *c);
    //     set_cache_unread(modified, *c);
      // }
    m_registry.update<T>(modified, removed, created);
  }

template<class T> void
Configuration::_reset_objects() noexcept
  {
    // auto j = m_cache_map.find(&T::s_class_name);

    // if (j != m_cache_map.end())
    //   {
    //     _unread_objects(static_cast<Cache<T>*>(j->second));
    //   }
    m_registry._reset_objects<T>();
  }

// // FIXME: Delete ME
// template<class T>
//   void
//   Configuration::_unread_objects(CacheBase* x) noexcept
//   {
//     Cache<T> *c = static_cast<Cache<T>*>(x);

//     for (auto& i : c->m_cache)
//       {
//         i.second->p_was_read = false;
//       }
//   }

// // FIXME: Delete ME
// template<class T> void
// Configuration::_rename_object(CacheBase* x, const std::string& old_id, const std::string& new_id) noexcept
// {
//   Cache<T> *c = static_cast<Cache<T>*>(x);

//   // rename template object
//   auto it = c->m_cache.find(old_id);
//   if (it != c->m_cache.end())
//     {
//       TLOG_DEBUG(3) << " * rename \'" << old_id << "\' to \'" << new_id << "\' in class \'" << T::s_class_name << "\')";
//       c->m_cache[new_id] = it->second;
//       c->m_cache.erase(it);

//       std::lock_guard<std::mutex> scoped_lock(it->second->m_mutex);
//       it->second->p_UID = new_id;
//     }

//   // rename generated objects if any
//   auto range = c->m_t_cache.equal_range(old_id);
//   for (auto it = range.first; it != range.second;)
//     {
//       T * o = dynamic_cast<T*>(it->second);
//       it = c->m_t_cache.erase(it);
//       c->m_t_cache.emplace(new_id, o);
//     }
// }


template<class T> void
Configuration::register_converter(AttributeConverter<T> * object) noexcept
  {
    std::lock_guard<std::mutex> scoped_lock(m_else_mutex);

    std::list<AttributeConverterBase*> * c = m_convert_map[typeid(T).name()];
    if (c == 0)
      {
        c = m_convert_map[typeid(T).name()] = new std::list<AttributeConverterBase*>();
      }

    c->push_back(object);
  }

template<class T>
  void
  Configuration::convert(T& value, const ConfigObject& obj, const std::string& attr_name) noexcept
  {
    conffwk::map<std::list<AttributeConverterBase*> *>::const_iterator l = m_convert_map.find(typeid(T).name());
    if (l != m_convert_map.end())
      {
        for (const auto& i : *l->second)
          {
            static_cast<AttributeConverter<T>*>(i)->convert(value, *this, obj, attr_name);
          }
      }
  }

template<class T>
  void
  Configuration::convert2(std::vector<T>& value, const ConfigObject& obj, const std::string& attr_name) noexcept
  {
    conffwk::map<std::list<AttributeConverterBase*> *>::const_iterator l = m_convert_map.find(typeid(T).name());
    if (l != m_convert_map.end())
      {
        for (auto& j : value)
          {
            for (const auto& i : *l->second)
              {
                static_cast<AttributeConverter<T>*>(i)->convert(j, *this, obj, attr_name);
              }
          }
      }
  }

// inline void
// CacheBase::increment_gets(Configuration& db) noexcept
// {
//   ++db.p_number_of_cache_hits;
// }




} // namespace conffwk
} // namespace dunedaq

#endif /*__DUNEDAQ_CONFFWK_CONFIGURATION_HXX__ */
