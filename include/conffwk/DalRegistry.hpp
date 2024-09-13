#ifndef __DUNEDAQ_CONFFWK_DALREGISTRY_HPP__
#define __DUNEDAQ_CONFFWK_DALREGISTRY_HPP__

#include "conffwk/map.hpp"
#include "conffwk/Errors.hpp"

namespace dunedaq {
namespace conffwk {

class DalObject;
class Configuration;


/**
 * @brief DalRegistry: A registry of DalObjects
 *        It provides a single interface to create, cache and manage DalObjecs
 * 
 */
class DalRegistry {

  friend class DalObject;

struct DalDomain {
  mutable std::mutex mutex; // mutex used to access template objects (i.e. generated DAL
  conffwk::map<DalObject*> cache;
};

public:
  /**
   * @brief Construct a new Dal Registry object
   * 
   * @param confdb Reference to a Configuration object;
   */
  DalRegistry(Configuration& confdb);
  ~DalRegistry();

  Configuration& configuration() { return m_confdb; }
  
  /**
   * 
   * \brief Clear the content of the registy
   */
  void clear();

  DalObject* get(ConfigObject& obj, bool upcast_unregistered=false);
  std::vector<const DalObject*> get(std::vector<ConfigObject>& objs, bool upcast_unregistered=false);

  /**
  *  \brief Get template object from cache by conffwk object.
  *
  *  The method searches an object with id of given conffwk object within the cache.
  *  If found, the method sets given conffwk object as implementation of the template
  *  object and returns pointer on the template object.
  *  If there is no such object in cache, then it is created from given conffwk object.
  *
  *  In case of success, the new object is put into cache and pointer to the object is returned.
  *  If there is no such object for given template class, then \b null pointer is returned.
  *
  *  \param conffwk         the configuration object
  *  \param obj            the conffwk object used to set for the template object
  *  \param init_children  if true, the referenced objects are initialized (only applicable during creation of new object)
  *  \param init_object    if true, the object's attributes and relationships are read(only applicable during creation of new object)
  *
  *  \return Return pointer to object.
  *
  *  \throw dunedaq::conffwk::Generic is no such class for loaded configuration DB schema or in case of an error
  */
  template<class T> 
  T * get(ConfigObject& obj, bool init_children=false, bool init_object=true);


  /**
  *  \brief Get template object from cache by object's ID.
  *
  *  The method searches an object with given id within the cache.
  *  If found, the method returns pointer on it.
  *  If there is no such object in cache, there is an attempt to create new object.
  *  In case of success, the new object is put into cache and pointer to the object is returned.
  *  If there is no such object for given template class, then \b null pointer is returned.
  *
  *  \param conffwk         the configuration object
  *  \param name           object identity
  *  \param init_children  if true, the referenced objects are initialized (only applicable during creation of new object)
  *  \param init_object    if true, the object's attributes and relationships are read(only applicable during creation of new object)
  *  \param rlevel         optional references level to optimize performance (defines how many objects referenced by given object have also to be read to the implementation cache during creation of new object)
  *  \param rclasses       optional array of class names to optimize performance (defines which referenced objects have to be read to the implementation cache during creation of new object)
  *
  *  \return Return pointer to object. It can be \b null, if there is no such object found.
  *
  *  \throw dunedaq::conffwk::Generic is no such class for loaded configuration DB schema or in case of an error
  */
  template<class T>
  T * get(const std::string& name, bool init_children=false, bool init_object=true, unsigned long rlevel = 0, const std::vector<std::string> * rclasses = nullptr);


  /**
  *  \brief Find template object using ID.
  *
  *  The method is suitable for generated template objects.
  *
  *  In case of success, the new object is put into cache and pointer to the object is returned.
  *  If there is no such object for given template class, then \b null pointer is returned.
  *
  *  \param id             ID of generated object
  *
  *  \return Return pointer to object.
  *
  *  \throw dunedaq::conffwk::Generic is no such class for loaded configuration DB schema or in case of an error
  */
  template<class T> 
  T * find(const std::string & id);

  /**
   *  \brief Checks validity of pointer to an objects of given user class.
   *
   *  Check if the pointer to the object is a valid pointer in the cache.
   *  Dangling pointers to removed objects may appear after notification.
   *
   *  \return Return \b true if the pointer is valid and \b false otherwise.
   */
  template<class T>
  bool is_valid(const T * object) noexcept;

  /**
   *  \brief Update cache of objects in case of modification.
   *
   *  Only is called, when a user subscription to related class is set.
   *  It is used by automatically generated data access libraries.
   *
   *  \param modified  vector of modified objects of given user class (objects to be re-read in cache)
   *  \param removed   vector of removed objects of given user class (objects to be removed from cache)
   *  \param created   vector of created objects of given user class (objects to be reset in cache, if they were removed)
   */
  template<class T>
  void update(const std::vector<std::string>& modified,
              const std::vector<std::string>& removed,
              const std::vector<std::string>& created);

  void update(const std::string& class_name,
              const std::vector<std::string>& modified,
              const std::vector<std::string>& removed,
              const std::vector<std::string>& created);

  /**
   * 
   * \brief Set the status of all objects in cache to unread
   * 
   */
  void unread_all();

  /**
   *  \brief Get signle value of object's relation and instantiate result with it (multi-thread safe).
   *
   *  The method is used by the code generated by the genconffwk utility.
   *
   *  \param obj   object
   *  \param name  name of the relationship
   *  \param init  if true, the object and it's referenced objects are initialized
   *
   *  \return Return non-null pointer to object of user class in case if
   *  relationship with such name exists and it's value is set.
   *  Otherwise the method returns 0.
   *
   *  \throw dunedaq::conffwk::Generic in case of a problem (e.g. no relationship with such name, plug-in specific problem)
   */

  template<class T>
  const T* _ref(ConfigObject& obj, const std::string& name, bool read_children);

  /**
   *  \brief Get multiple values of object's relation and instantiate result with them (multi-thread safe).
   *
   *  The method is used by the code generated by the genconffwk utility.
   *
   *  \param obj       object
   *  \param name      name of the relationship
   *  \param objects   returned value
   *  \param init      if true, the objects and their referenced objects are initialized
   *
   *  \throw dunedaq::conffwk::Generic in case of a problem (e.g. no relationship with such name, plug-in specific problem)
   */
  template<class T> 
  void _ref(ConfigObject& obj, const std::string& name, std::vector<const T*>& results, bool read_children);

  /**
   *  \brief Update state of objects after abort operations.
   *
   *  It is used by automatically generated data access libraries.
   */
  template<class T> 
  void _reset_objects();

  /**
   *  \brief Rename object of given template class (multi-thread unsafe).
   *
   *  Is used by automatically generated data access libraries when an object has been renamed by user's code.
   *  Should not be explicitly used by user.
   *
   *  The method is used by the unread_all_objects() method.
   *  \param  cache_ptr pointer to the cache of template object of given template class (has to be downcasted)
   *  \param  old_id old object ID
   *  \param  new_id new object ID
   */
  void _rename_object(std::string class_name, std::string old_id, std::string new_id);

  /**
   * @brief Update the internal class domains map
   * 
   */
  void update_class_maps() {
    this->update_class_domain_map();
  }


private:

  Configuration& m_confdb;

  mutable std::mutex m_mutex; // mutex used to access template objects (i.e. generated DAL)

  std::deque<std::set<std::string>> find_class_domains();

  void update_class_domain_map();

  conffwk::fmap<uint> m_class_domain_map;

  std::unordered_map<uint,DalDomain> m_cache_domains;

};

} // namespace conffwk
} // namespace dunedaq

#endif /* __DUNEDAQ_CONFFWK_DALREGISTRY_HPP__ */
