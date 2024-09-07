#include "conffwk/DalObject2g.hpp"

namespace dunedaq {
namespace conffwk {


bool
DalObject2g::get_rel_objects(const std::string &name, bool upcast_unregistered, std::vector<const DalObject2g*> &objs) const
{
//   std::vector<conffwk::ConfigObject> c_objs;

//   if (const_cast<conffwk::ConfigObject*>(&p_obj)->rel(name, c_objs))
//     {
//       std::lock_guard<std::mutex> scoped_lock(p_db.m_tmpl_mutex);
//       p_db.make_dal_objects(c_objs, upcast_unregistered).swap(objs);
//       return true;
//     }

  return false;
}


bool
DalObject2g::get_algo_objects(const std::string &name, std::vector<const DalObject2g*> &objs) const
{
//   const std::string &suitable_dal_class = DalFactory::instance().class4algo(p_db, class_name(), name);

//   TLOG_DEBUG(2) << "suitable class for algorithm " << name << " on object " << this << " is " << suitable_dal_class;

//   if (!suitable_dal_class.empty())
//     if (const DalObject2g *obj = p_db.make_dal_object(const_cast<conffwk::ConfigObject&>(p_obj), UID(), suitable_dal_class))
//       {
//         obj->get(name, false).swap(objs);
//         return true;
//       }

  return false;
}



  // DalObject2g helper

void
DalObject2g::p_null(std::ostream &s)
{
  s << "(null)";
}

void
DalObject2g::p_rm(std::ostream &s)
{
  s << "(deleted object)";
}

void
DalObject2g::p_error(std::ostream &s, dunedaq::conffwk::Exception &ex)
{
  s << "ERROR in generated DAL print method:\n\twas caused by: " << ex << std::endl;
}

void
DalObject2g::p_hdr(std::ostream &s, unsigned int indent, const std::string &cl, const char *nm) const
{
  const std::string str(indent, ' ');
  s << str;
  if (nm)
    s << nm << ' ';
  s << cl << " object:\n" << str << "  id: \'" << UID() << "\', class name: \'" << DalObject2g::class_name() << "\'\n";
}

void DalObject2g::throw_init_ex(dunedaq::conffwk::Exception& ex)
{
  std::ostringstream text;
  text << "failed to init " << this << ":\n\twas caused by: " << ex << std::endl;
  p_was_read = false;
  throw dunedaq::conffwk::Generic (ERS_HERE, text.str().c_str());
}

void DalObject2g::throw_get_ex(const std::string& what, const std::string& class_name, const DalObject2g * obj)
{
  std::ostringstream text;
  text << "cannot find relationship or algorithm \"" << what << "\" in c++ class \"" << class_name << "\" for object " << obj;
  throw dunedaq::conffwk::Generic(ERS_HERE, text.str().c_str());
}
    
    
std::ostream&
operator<<(std::ostream& s, const DalObject2g * obj)
{
  if (obj == nullptr)
    DalObject2g::p_null(s);
  else if (obj->is_deleted())
    s << "(deleted object " << obj->UID() << '@' << obj->class_name() << ')';
  else
    s << '\'' << obj->UID() << '@' << obj->class_name() << '\'';

  return s;
}




    
} // namespace conffwk
    
} // namespace dunedaq
