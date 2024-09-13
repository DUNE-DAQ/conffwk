#if !defined(__DUNEDAQ_CONFFWK_DALFACTORY_HXX__)
#define __DUNEDAQ_CONFFWK_DALFACTORY_HXX__

namespace dunedaq{
namespace conffwk{

/** register DAL object creator by class name*/
template<class T>
void
DalFactory::register_dal_class(const std::string & name, const std::set<std::string>& algorithms)
{
  std::lock_guard<std::mutex> scoped_lock(m_class_mutex);

  TLOG_DEBUG(1) << "register class " << name;

  if (m_classes.emplace(name, DalFactoryFunctions(boost::compute::identity<T>(), algorithms)).second == false)
    {
      TLOG_DEBUG(0) << "class " << name << " was already registered";
    }
}

/** register DAL object creator by class name*/
template<class T>
void
DalFactory::register_dal_class_2g(const std::string & name)
{
  std::lock_guard<std::mutex> scoped_lock(m_class_mutex);

  TLOG_DEBUG(50) << "register class " << name;

  if (m_creators.emplace(name, [](conffwk::DalRegistry& db, const conffwk::ConfigObject& o){ return new T(db, o);}).second == false)
    {
      TLOG() << "class " << name << " was already registered";
    }
}

} // namespace conffwk
} // namespace dunedaq


#endif // __DUNEDAQ_CONFFWK_DALFACTORY_HXX__
